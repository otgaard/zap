//
// Created by Darren Otgaar on 2017/08/19.
//

#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <maths/io.hpp>
#include <host/GLFW/application.hpp>
#include <graphics/graphics3/g3_types.hpp>
#include <renderer/renderer.hpp>
#include <graphics/generators/generator.hpp>
#include <renderer/scene_graph/node.hpp>
#include <renderer/scene_graph/visual.hpp>
#include <graphics/generators/geometry/geometry3.hpp>
#include <renderer/shader_builder.hpp>

const int MAX_OBJECTS = 100;

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::renderer;
using namespace zap::scene_graph;

// Geometry Generators

using p3t2_tri_geo3 = generators::geometry3<vtx_p3t2_t, primitive_type::PT_TRIANGLES>;
using p3n3t2_tri_geo3 = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>;
using p3n3t2_ts_geo3 = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLE_STRIP>;
using p3n3t2_tf_geo3 = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLE_FAN>;

// Scene Graph

using spatial_t = spatial<transform4f, geometry::spheref>;
using node_t = node<spatial_t, spatial_t*>;
using visual_t = visual<spatial_t>;

// Shader Builders

using shdr_settings = builder_task<1, 10, 2>;

class scene_graph_app : public application {
public:
    scene_graph_app() : application{"scene_graph", 1280, 800} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

private:
    zap::generator gen_;
    zap::renderer::renderer rndr_;
    std::vector<std::unique_ptr<mesh_base>> meshes_;
    std::vector<std::unique_ptr<render_context>> contexts_;
    std::vector<visual_t> visuals_;
    std::vector<render_args> arguments_;
    node_t graph_;

    using controller_fnc = std::function<void(float)>;
    std::vector<controller_fnc> controllers_;
    std::vector<texture> textures_;
    camera cam_;
};

bool scene_graph_app::initialise() {
    if(!zap::engine::init()) {
        LOG_ERR("Zap Engine Failed to initialise");
        return false;
    }

    visuals_.reserve(MAX_OBJECTS);
    arguments_.reserve(MAX_OBJECTS);
    contexts_.reserve(10);

    if(!rndr_.initialise()) {
        LOG_ERR("Renderer failed to initialise");
        return false;
    }

    if(!gen_.initialise()) {
        LOG_ERR("Generator failed to initialise");
        return false;
    }

    render_task req{512, 512, render_task::basis_function::USER_FUNCTION};
    req.project = render_task::projection::CUBE_MAP;
    req.scale.set(10.f, 1.f);
    float min = std::numeric_limits<float>::max(), max = -min;
    auto skybox_tex = gen_.render_cubemap(req, [&max, &min](float x, float y, float z, generator& gen) {
        //int ix = maths::floor(x), iy = maths::floor(y), iz = maths::floor(z);
        float value = .5f + .5f*gen.snoise(x, y, z, 1.f);
        if(value < min) min = value;
        if(value > max) max = value;

        vec3b colour = lerp(vec3b(20, 0, 0), vec3b(55, 20, 55), value);
        return rgb888_t{colour};
    });

    LOG(LOG_RED, "Min:", min, "Max:", max);

    textures_.emplace_back(std::move(skybox_tex));

    shdr_settings task;
    task.method = shdr_settings::lighting_method::LM_NONE;
    task.diffuse_map = texture_type::TT_CUBE_MAP;
    auto cubemap_context = shader_builder::build_basic_lights(task);
    auto skybox_ptr = p3n3t2_tri_geo3::make_mesh(p3n3t2_tri_geo3::make_skybox(vec3f{1.f, 1.f, 1.f}));
    cubemap_context->add_texture(&textures_[0]);
    cubemap_context->set_texture_unit("diffuse_map", 0);
    visuals_.emplace_back(skybox_ptr.get(), cubemap_context.get());
    visuals_.back().uniform_scale(50.f);
    arguments_.emplace_back(cubemap_context.get());
    arguments_.back().add_parameter("colour", vec4f{1.f, 1.f, 1.f, 1.f});
    arguments_.back().add_parameter("diffuse_map", 0);

    contexts_.emplace_back(std::move(cubemap_context));
    meshes_.emplace_back(std::move(skybox_ptr));

    task.method = shdr_settings::lighting_method::LM_NONE;      // Flat colour only
    task.diffuse_map = shdr_settings::texture_type::TT_NONE;

    // Build some planets
    auto flat_context = shader_builder::build_basic_lights(task);
    auto sphere_ptr = p3n3t2_tri_geo3::make_mesh(p3n3t2_tri_geo3::make_UVsphere(30, 60, .5f, false));

    visuals_.emplace_back(sphere_ptr.get(), flat_context.get());
    arguments_.emplace_back(flat_context.get());
    arguments_.back().add_parameter("colour", vec4f{.6f, 0.6f, 1.f, 1.f});

    visuals_.emplace_back(sphere_ptr.get(), flat_context.get());
    visuals_.back().translate(vec3f{1.f, 0.f, 0.f});
    visuals_.back().uniform_scale(.2f);
    arguments_.emplace_back(flat_context.get());
    arguments_.back().add_parameter("colour", vec4f{1.f, 1.f, 0.f, 1.f});

    // Add a controller
    controllers_.emplace_back([this](float dt) {
        static float rotation = 0.f;
        auto& v = visuals_[0];
        //auto& a = arguments_[0];

        rotation = wrap(rotation+.1f*dt, -TWO_PI<float>, TWO_PI<float>);
        //float parm = (rotation + TWO_PI<float>)/(2.f*TWO_PI<float>);
        v.rotate(make_rotation(vec3f{0.f, 1.f, 0.f}, rotation));
    });

    controllers_.emplace_back([this](float dt) {
        static float rotation = 0.f;
        auto& v = visuals_[2];
        auto& a = arguments_[2];

        rotation = wrap(rotation+dt, -TWO_PI<float>, TWO_PI<float>);
        float parm = (rotation + TWO_PI<float>)/(2.f*TWO_PI<float>);

        v.translate(make_rotation(vec3f{0.f, 1.f, 0.f}, rotation) * vec3f{1.f, 0.f, 0.f});
        a.add_parameter("colour", lerp(vec4f{1.f, 0.f, 0.f, 1.f}, vec4f{1.f, 1.f, 0.f, 1.f}, parm));
    });

    // Store created contexts
    contexts_.emplace_back(std::move(flat_context));

    // Store created meshes
    meshes_.emplace_back(std::move(sphere_ptr));

    assert(visuals_.size() < MAX_OBJECTS && "visuals_ must not reallocate during or after initialisation!");
    return visuals_.size() < MAX_OBJECTS;
}

void scene_graph_app::update(double t, float dt) {
    for(auto& controller : controllers_) controller(dt);

    for(int i = 0; i != visuals_.size(); ++i) {
        if(visuals_[i].is_dirty()) {
            arguments_[i].add_parameter("mvp_matrix", cam_.proj_view() * visuals_[i].world_transform().gl_matrix());
        }
    }
}

void scene_graph_app::draw() {
    for(int i = 0; i != visuals_.size(); ++i) {
        rndr_.draw(visuals_[i], arguments_[i]);
    }
}

void scene_graph_app::shutdown() {
}

void scene_graph_app::on_resize(int width, int height) {
    cam_.viewport(0, 0, width, height);
    cam_.frustum(45.f, width/float(height), .5f, 200.f);
    cam_.world_pos(0.f, 0.f, 10.f);
    cam_.look_at(vec3f{0.f, 0.f, 0.f});
    for(int i = 0; i != visuals_.size(); ++i) {
        arguments_[i].add_parameter("mvp_matrix", cam_.proj_view() * visuals_[i].world_transform().gl_matrix());
    }
}

int main(int argc, char* argv[]) {
    scene_graph_app app{};
    app_config config;
    config.resizeable_window = true;
    return app.run(config);
}

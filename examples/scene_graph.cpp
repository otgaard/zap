//
// Created by Darren Otgaar on 2017/08/19.
//

#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <maths/io.hpp>
#include <host/GLFW/application.hpp>
#include <graphics3/g3_types.hpp>
#include <renderer/renderer.hpp>
#include <generators/generator.hpp>
#include <scene_graph/node.hpp>
#include <scene_graph/visual.hpp>
#include <generators/geometry/geometry3.hpp>
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
    scene_graph_app() : application{"scene_graph", 1920, 1080, false} { }

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

    camera cam_;
};

bool scene_graph_app::initialise() {
    visuals_.reserve(MAX_OBJECTS);
    arguments_.reserve(MAX_OBJECTS);

    if(!rndr_.initialise()) {
        LOG_ERR("Renderer failed to initialise");
        return false;
    }

    shdr_settings task;
    task.method = shdr_settings::lighting_method::LM_NONE;      // Flat colour only
    task.diffuse_map = shdr_settings::texture_type::TT_NONE;

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
        auto& v = visuals_[1];
        auto& a = arguments_[1];

        rotation = wrap(rotation+dt, -TWO_PI<float>, TWO_PI<float>);
        float parm = (rotation + TWO_PI<float>)/(2.f*TWO_PI<float>);

        v.translate(make_rotation(vec3f{0.f, 1.f, 0.f}, rotation) * vec3f{1.f, 0.f, 0.f});
        a.add_parameter("colour", lerp(parm, vec4f{1.f, 0.f, 0.f, 1.f}, vec4f{1.f, 1.f, 0.f, 1.f}));
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
    cam_.frustum(45.f, width/float(height), .5f, 100.f);
    cam_.world_pos(0.f, 0.f, 10.f);
    cam_.look_at(vec3f{0.f, 0.f, 0.f});
    for(int i = 0; i != visuals_.size(); ++i) {
        arguments_[i].add_parameter("mvp_matrix", cam_.proj_view() * visuals_[i].world_transform().gl_matrix());
    }
}

int main(int argc, char* argv[]) {
    scene_graph_app app{};
    return app.run();
}

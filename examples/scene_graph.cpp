//
// Created by Darren Otgaar on 2017/07/27.
//

/**
 * Tests the scene graph classes
 */

#include <tools/log.hpp>
#include <maths/io.hpp>
#include <host/GLFW/application.hpp>

#include <scene_graph/node.hpp>
#include <scene_graph/visual.hpp>

#include <renderer/camera.hpp>
#include <generators/geometry/geometry3.hpp>
#include <renderer/renderer.hpp>
#include <generators/generator.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::renderer;
using namespace zap::scene_graph;

using spheref = geometry::sphere<float>;
using spatial_t = spatial<transform4f, spheref>;
using node_t = node<spatial_t>;
using visual_t = visual<spatial_t>;
using p3n3t2_geo3 = generators::geometry3<vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>;

using cam_block = uniform_block<
    core::cam_world_to_view<mat4f>,
    core::cam_projection<mat4f>,
    core::cam_proj_view<mat4f>,
    core::viewport<vec4i>,
    core::eye_position<vec3f>,
    core::eye_dir<vec3f>
>;
using cam_ubuffer = uniform_buffer<cam_block>;

using model_block = uniform_block<
    core::mvp_matrix<mat4f>,
    core::mv_matrix<mat4f>,
    core::model_matrix<mat4f>,
    core::normal_matrix<mat4f>
>;
using model_ubuffer = uniform_buffer<model_block>;

const char* const basic_vshdr = GLSL(
    layout (std140) uniform camera_ublock {
        mat4 cam_view;
        mat4 cam_projection;
        mat4 cam_proj_view;
        vec4 viewport;
        vec3 eye_position;
        vec3 eye_dir;
    };

    layout (std140) uniform model_ublock {
        mat4 mvp_matrix;
        mat4 mv_matrix;
        mat4 model_matrix;
        mat4 normal_matrix;
    };

    in vec3 position;
    in vec3 normal;
    in vec2 texcoord1;

    out vec3 pos;
    out vec3 nor;
    out vec2 tex;

    void main() {
        pos = (mv_matrix * vec4(position, 1.)).xyz;
        nor = normalize((normal_matrix * vec4(normal, 0.)).xyz);
        tex = texcoord1;
        gl_Position = mvp_matrix * vec4(position, 1.0);
    }
);

const char* const basic_fshdr = GLSL(
    layout (std140) uniform camera_ublock {
        mat4 cam_view;
        mat4 cam_projection;
        mat4 cam_proj_view;
        vec4 viewport;
        vec3 eye_position;
        vec3 eye_dir;
    };

    uniform vec4 colour = vec4(1., 1., 1., 1.);
    uniform vec3 light_dir = vec3(0., 1., 0.);

    in vec3 pos;
    in vec3 nor;
    in vec2 tex;

    out vec4 frag_colour;
    void main() {
        vec3 vP = normalize(eye_position - pos);
        float Ld = max(0, dot(light_dir, nor));
        vec3 H = normalize(light_dir + vP);
        float Ls = pow(max(0., dot(H, nor)), 100.) * Ld;
        frag_colour = Ld * colour + Ls * vec4(1., 1., 1., 1.);
    }
);

class scene_graph_test : public application {
public:
    scene_graph_test() : application{"scene_graph_test", 1280, 900, false}, cam_{true} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

protected:
    camera cam_;
    std::vector<std::unique_ptr<mesh_base>> meshes_;
    std::vector<render_context> contexts_;
    cam_ubuffer camera_ublock_;
    model_ubuffer model_ublock_;
    zap::renderer::renderer rndr_;
    std::unique_ptr<render_state> rndr_state_;
    generator tex_gen_;
};

bool scene_graph_test::initialise() {
    clear(0.f, 0.f, 0.f, 0.f);

    if(!tex_gen_.initialise()) {
        LOG_ERR("Failed to initialise texture generator");
        return false;
    }

    if(!camera_ublock_.allocate() || !model_ublock_.allocate()) {
        LOG_ERR("Failed to allocate uniform block");
        return false;
    }

    camera_ublock_.bind();
    if(!camera_ublock_.initialise()) {
        LOG_ERR("Failed to initialise cam_uniform");
        return false;
    }
    camera_ublock_.release();

    model_ublock_.bind();
    if(!model_ublock_.initialise()) {
        LOG_ERR("Failed to initialise model_uniform");
        return false;
    }
    model_ublock_.release();

    auto cube = p3n3t2_geo3::make_cube(vec3f(.1f, .5f, .1f));
    auto cube_mesh = make_mesh<vtx_p3n3t2_t, primitive_type::PT_TRIANGLES>(cube);
    meshes_.emplace_back(std::move(cube_mesh));

    auto sphere = p3n3t2_geo3::make_UVsphere<float, uint32_t>(30, 60, .2f, false);
    auto sphere_mesh = make_mesh<vtx_p3n3t2_t, primitive_type::PT_TRIANGLES, uint32_t>(sphere);
    meshes_.emplace_back(std::move(sphere_mesh));

    contexts_.emplace_back(basic_vshdr, basic_fshdr);
    contexts_.back().add_uniform_buffer("camera_ublock", &camera_ublock_, "model_ublock", &model_ublock_);
    if(!contexts_.back().initialise()) {
        LOG_ERR("Failed to initialise context");
        return false;
    }
#if defined(__APPLE__) || defined(_WIN32)
    rndr_state_ = std::make_unique<render_state>(true, true);
#else
    rndr_state_ = std::unique_ptr<render_state>(new render_state{true, true});
#endif
    contexts_.back().set_state(rndr_state_.get());
    if(!rndr_.initialise()) {
        LOG_ERR("Error initialising renderer");
        return false;
    }

    gl_error_check();
    return true;
}

void scene_graph_test::on_resize(int width, int height) {
    cam_.viewport(0, 0, width, height);
    cam_.world_pos(vec3f{1.f, 1.f, 3.f});
    cam_.look_at(vec3f{0.f, 0.f, 0.f});
    cam_.frustum(45.f, width/float(height), .5f, 100.f);
    camera_ublock_.bind();
    if(camera_ublock_.map(buffer_access::BA_READ_WRITE)) {
        camera_ublock_.ref().cam_world_to_view = cam_.world_to_view();
        camera_ublock_.ref().cam_projection = cam_.projection();
        camera_ublock_.ref().cam_proj_view = cam_.proj_view();
        camera_ublock_.ref().viewport = vec4i{int(cam_.viewport()[0]), int(cam_.viewport()[1]), int(cam_.viewport()[2]), int(cam_.viewport()[3])};
        camera_ublock_.ref().eye_position = cam_.world_pos();
        camera_ublock_.ref().eye_dir = -cam_.dir();
        contexts_.back().set_parameter("light_dir", cam_.world_to_view() * normalise(vec3f{1.f, 1.f, 1.f}));
        camera_ublock_.unmap();
    }
    camera_ublock_.release();
    gl_error_check();
}

void scene_graph_test::update(double t, float dt) {
    static float rot = 0.f;
    model_ublock_.bind();
    if(model_ublock_.map(buffer_access::BA_READ_WRITE)) {
        model_ublock_.ref().model_matrix = make_rotation(vec3f{0.f, 1.f, 0.f}, rot) * make_scale(2.f, 2.f, 2.f);
        model_ublock_.ref().mv_matrix = cam_.world_to_view() * model_ublock_.ref().model_matrix;
        model_ublock_.ref().mvp_matrix = cam_.projection() * model_ublock_.ref().mv_matrix;
        model_ublock_.ref().normal_matrix = transpose(model_ublock_.ref().mv_matrix.inverse());
        model_ublock_.unmap();
    }
    model_ublock_.release();

    gl_error_check();
}

void scene_graph_test::draw() {
    const vec4f colours[2] = {vec4f{1.f, 1.f, 0.f, 1.f}, vec4f{0.f, 0.f, 1.f, 1.f}};
    contexts_.back().bind(rndr_);
    int counter = 0;
    for(auto& mesh : meshes_) {
        mesh->bind();
        contexts_.back().set_parameter("colour", colours[counter++]);
        mesh->draw();
        mesh->release();
    }
    contexts_.back().release(rndr_);
    gl_error_check();
}

void scene_graph_test::shutdown() {
    gl_error_check();
}

int main(int argc, char* argv[]) {
    scene_graph_test app;
    return app.run();
}

// Generators
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
#include <scene_graph/bound.hpp>
#include <scene_graph/visual.hpp>

#include <renderer/camera.hpp>
#include <generators/geometry/geometry3.hpp>

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
using p3_geo3 = generators::geometry3<vtx_p3_t, primitive_type::PT_TRIANGLES>;

const char* const basic_vshdr = GLSL(
    uniform mat4 PVM;
    in vec3 position;
    void main() {
        gl_Position = PVM * vec4(position, 1.0);
    }
);

const char* const basic_fshdr = GLSL(
    out vec4 frag_colour;
    void main() {
        frag_colour = vec4(1., 1., 1., 1.);
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
};

bool scene_graph_test::initialise() {
    clear(0.f, 0.f, 0.f, 0.f);

    auto cube = p3_geo3::make_cube(vec3f(1.f, 1.f, 1.f));
    auto cube_mesh = make_mesh<vtx_p3_t, primitive_type::PT_TRIANGLES>(cube);

    auto sphere = p3_geo3::make_UVsphere<float, uint32_t>(30, 60, 1.f, true);
    auto sphere_mesh = make_mesh<vtx_p3_t, primitive_type::PT_TRIANGLES, uint32_t>(sphere);

    gl_error_check();
    return true;
}

void scene_graph_test::on_resize(int width, int height) {
    gl_error_check();
}

void scene_graph_test::update(double t, float dt) {
    gl_error_check();
}

void scene_graph_test::draw() {
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
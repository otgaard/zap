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

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::scene_graph;

using spheref = geometry::sphere<float>;
using spatial_t = spatial<transform4f, spheref>;
using node_t = node<spatial_t>;
using visual_t = visual<spatial_t>;

class scene_graph_test : public application {
public:
    scene_graph_test() : application{"scene_graph_test", 1280, 900, false}, cam_{true} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

    void draw_scene();

protected:
    camera cam_;
};

bool scene_graph_test::initialise() {
    clear(0.f, 0.f, 0.f, 0.f);
    gl_error_check();
    return true;
}

void scene_graph_test::on_resize(int width, int height) {
    gl_error_check();
}

void scene_graph_test::update(double t, float dt) {
    gl_error_check();
}

void scene_graph_test::draw_scene() {
    gl_error_check();
}

void scene_graph_test::draw() {
    draw_scene();
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
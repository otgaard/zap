#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include "host/GLFW/application.hpp"
#include <graphics2/plotter/plotter.hpp>
#include <renderer/camera.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::renderer;

class plotter : public application {
public:
    plotter() : application("plotter", 1280, 768, false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    camera cam_;
    graphics::plotter plot_;
};

bool plotter::initialise() {
    cam_.viewport(0, 0, 1280, 768);
    cam_.frustum(0, 1280, 0, 768, 0, 1);
    cam_.frame(vec3f(0, 1, 0), vec3f(0, 0, -1), vec3f(0, 0, 0));

    plot_.set_grid(graphics::plotter::grid(vec2f(-400,400), vec2f(1.f, .5f), vec2f(-400,400), vec2f(1.f, .5f)));

    if(!plot_.initialise()) {
        LOG_ERR("Failed to initialise graphics::plotter");
        return false;
    }

    return true;
}

void plotter::on_resize(int width, int height) {
}

void plotter::on_mousemove(double x, double y) {
}

void plotter::on_mousewheel(double xinc, double yinc) {
}

void plotter::update(double t, float dt) {
    plot_.update(t, dt);
}

void plotter::draw() {
    plot_.draw(cam_);
}

void plotter::shutdown() {
}

int main(int argc, char* argv[]) {
    plotter app;
    return app.run();
}

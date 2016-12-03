#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <maths/algebra.hpp>
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

    graphics::sampler1D<float> live_;
};

bool plotter::initialise() {
    cam_.viewport(0, 0, 1280, 768);
    cam_.frustum(0, 1280, 0, 768, 0, 1);
    cam_.frame(vec3f(0, 1, 0), vec3f(0, 0, -1), vec3f(0, 0, 0));

    line_width(.5f);

    plot_.set_grid(graphics::plotter::grid(vec2f(0,1.f), vec2f(.1f, .05f), vec2f(0,1.f), vec2f(.1f, .05f)));

    if(!plot_.initialise()) {
        LOG_ERR("Failed to initialise graphics::plotter");
        return false;
    }

    plot_.world_transform.translate(vec2f(1280/2.f - 500/2.f, 768/2.f - 500/2.f));
    plot_.world_transform.uniform_scale(500.f);

    // Set up a sampler & and plot
    std::vector<float> data(10);
    for(int i = 0; i != data.size(); ++i) data[i] = rand()/float(RAND_MAX);

    auto sampler = graphics::sampler1D<float>(data, graphics::interpolators::cubic<float>);
    plot_.add_plot(sampler, 1000, vec3b(255, 255, -0));

    sampler = graphics::sampler1D<float>(data, graphics::interpolators::linear<float>);
    plot_.add_plot(sampler, 1000, vec3b(0, 0, 255));

    sampler = graphics::sampler1D<float>(data, graphics::interpolators::nearest<float>);
    plot_.add_plot(sampler, 1000, vec3b(255, 0, 0));

    live_.data.resize(100);
    for(int i = 0; i != live_.data.size(); ++i) {
        live_.data[i] = rand()/float(RAND_MAX);
    }
    live_.fnc = graphics::interpolators::cubic<float>;
    live_.inv_u = 1.f/(live_.data.size()-1);
    plot_.add_live_plot(&live_, 1000, vec3b(255,255,255));

    return true;
}

void plotter::on_resize(int width, int height) {
}

void plotter::on_mousemove(double x, double y) {
}

void plotter::on_mousewheel(double xinc, double yinc) {
}

void plotter::update(double t, float dt) {
    static float trigger = 0.f;

    trigger += dt;
    if(trigger > 0.01f) {
        for(int i = 0, end = (int) live_.data.size() - 1; i != end; ++i) {
            live_.data[i] = live_.data[i + 1];
        }
        live_.data[live_.data.size() - 1] = 0.25f*rand() / float(RAND_MAX) + 0.5f - 0.125f;
        plot_.update(t, dt);
        trigger = 0.f;
    }
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

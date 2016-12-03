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
    graphics::sampler1D<float, decltype(graphics::interpolators::linear<float>)> live_;
};

bool plotter::initialise() {
    cam_.viewport(0, 0, 1280, 768);
    cam_.frustum(0, 1280, 0, 768, 0, 1);
    cam_.frame(vec3f(0, 1, 0), vec3f(0, 0, -1), vec3f(0, 0, 0));

    application::clear(0.15f, 0.15f, 0.15f, 1.f);

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
    for(int i = 0; i != data.size(); ++i) data[i] = (1.1f - std::sinf(i*TWO_PI/(data.size()-1)))*.45f;

    auto sampler2 = graphics::sampler1D<float, decltype(graphics::interpolators::nearest<float>)>(data, graphics::interpolators::cosine<float>);
    plot_.add_plot(sampler2, 1000, vec3b(255, 0, 255));

    sampler2 = graphics::sampler1D<float, decltype(graphics::interpolators::nearest<float>)>(data, graphics::interpolators::linear<float>);
    plot_.add_plot(sampler2, 1000, vec3b(0, 0, 255));

    sampler2 = graphics::sampler1D<float, decltype(graphics::interpolators::nearest<float>)>(data, graphics::interpolators::nearest<float>);
    plot_.add_plot(sampler2, 1000, vec3b(255, 0, 0));

    auto sampler1 = graphics::sampler1D<float, decltype(graphics::interpolators::cubic<float>)>(data, graphics::interpolators::cubic<float>);
    plot_.add_plot(sampler1, 1000, vec3b(0, 255, 0));

    data.resize(1000);
    for(int i = 0; i != 1000; ++i) data[i] = (1.1f - std::sinf(i*TWO_PI/999))*.45f;
    auto sampler3 = graphics::sampler1D<float, decltype(graphics::interpolators::nearest<float>)>(data, graphics::interpolators::linear<float>);
    plot_.add_plot(sampler3, 1000, vec3b(255,255,255));

    live_.data.resize(50, 0.f);
    live_.fnc = graphics::interpolators::linear<float>;
    live_.inv_u = 1.f/49.f;

    return true;
}

void plotter::on_resize(int width, int height) {
    cam_.viewport(0, 0, width, height);
    cam_.frustum(0, width, 0, height, 0, 1);
    float scale = std::max(width/2.f, height/2.f);
    plot_.world_transform.uniform_scale(scale);
    plot_.world_transform.translate(vec2f(width/2.f - scale/2, height/2.f - scale/2));
}

void plotter::on_mousemove(double x, double y) {
}

void plotter::on_mousewheel(double xinc, double yinc) {
}

void plotter::update(double t, float dt) {
    static float curr_t = 0.f;

    curr_t += dt;
    if(curr_t > TWO_PI) curr_t -= TWO_PI;

    std::copy(live_.data.begin()+1, live_.data.end(), live_.data.begin());
    live_.data.back() = (1.f - std::sinf(4*curr_t))*.5f;
    plot_.live_plot(live_, 1000, vec3b(255,255,0));
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

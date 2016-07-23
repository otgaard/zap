/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>

#include "shader_src.hpp"
#include "apps/application.hpp"

#include <engine/engine.hpp>
#include <engine/mesh.hpp>
#include <maths/algebra.hpp>
#include <engine/program.hpp>
#include <renderer/colour.hpp>
#include <graphics2/plotter2.hpp>
#include <maths/curves/hermite.hpp>
#include <core/enumfield.hpp>
#include <maths/transform.hpp>
#include <renderer/camera.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;

class zap_example : public application {
public:
    zap_example() : application("zap_example", 1280, 768, false) { }

    void initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    graphics2::plotter2 plotter;
    curves::hermite<float, vec2f> curve;
    transform3f trans;
    camera cam;
};

void zap_example::initialise() {
    curve.set_points(vec2f(-1.f,-1.f), vec2f(1.f,1.f));
    curve.set_tangents(vec2f(2,0), vec2f(2,0));

    plotter.initialise();
    plotter.sample_curve(curve, 0.f, 1.f);
    gl_error_check();

    cam.world_pos(vec3f(0,0,5));
    trans.rotate(make_rotation(2.3f));
    plotter.set_mv_matrix(cam.world_to_view() * trans.gl_matrix());
}

void zap_example::on_resize(int width, int height) {
    application::on_resize(width, height);
    cam.viewport(0, 0, width, height);
    cam.frustum(45.f, float(width)/height, 0.5f, 100.f);
    plotter.set_projection_matrix(cam.projection());
}

void zap_example::on_mousemove(double x, double y) {
    constexpr static vec3f N(0,0,1);
    if(x > sc_width_ || x < 0 || y > sc_height_ || y < 0) return;
    vec3f origin, dir;

    if(cam.pick_ray(int(x), int(y), origin, dir)) {
        auto view_pos = origin - dot(origin, N)/dot(dir, N) * dir;
        auto model_pos = cam.view_to_world() * trans.gl_inverse() * view_pos;

        curve.set_points(model_pos.xy(), vec2f(1.f, 1.f));
        plotter.sample_curve(curve, 0.f, 1.f);
    }
}

void zap_example::on_mousewheel(double xinc, double yinc) {
    LOG(xinc, yinc);
}

void zap_example::update(double t, float dt) {
}

void zap_example::draw() {
    line_width(1.0f);
    plotter.draw();
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}

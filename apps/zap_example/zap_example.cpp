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

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;

class zap_example : public application {
public:
    zap_example() : application("zap_example", 1280, 768, false) { }

    void initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;
    void on_mousemove(double x, double y) final;

protected:
    graphics2::plotter2 plotter;
    curves::hermite<float, vec2f> curve;
    mat4f proj_matrix;
    transform3f trans;
};

void zap_example::initialise() {
    curve.set_points(vec2f(-1.f,-1.f), vec2f(1.f,1.f));
    curve.set_tangents(vec2f(2,0), vec2f(2,0));

    plotter.initialise();
    plotter.sample_curve(curve, 0.f, 1.f);

    gl_error_check();
}

void zap_example::on_resize(int width, int height) {
    application::on_resize(width, height);

    const float hw = 2.f/width, hh = 2.f/height;

    proj_matrix = {
        hw,  0.f, 0.f,  -hw,
        0.f,  hh, 0.f,  -hh,
        0.f, 0.f, 2.f, -1.f,
        0.f, 0.f, 0.f,  1.f
    };

    plotter.set_projection_matrix(proj_matrix);

    //plotter.set_mv_matrix(make_scale(std::min(hw, hh), std::min(hw, hh), 1.f));
}

void zap_example::on_mousemove(double x, double y) {
    if(x > sc_width_ || x < 0 || y > sc_height_ || y < 0) return;

    auto T = trans.inv_affine();
    auto lP = T.transform(vec2f(x,-y) - .5f*vec2f(sc_width_, -sc_height_));

    curve.set_points(lP, vec2f(1.f, 1.f));
    plotter.sample_curve(curve, 0.f, 1.f);
}

static float scale = 1.f;

void zap_example::update(double t, float dt) {
    trans.uniform_scale(200.f);
    trans.rotate(make_rotation<float>(PI*scale/10.f));
    //trans.translate(vec2f(50*scale,25*scale));
    scale += dt;
    plotter.transform(trans.gl_matrix());
}

void zap_example::draw() {
    line_width(0.5f);
    plotter.draw();
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}

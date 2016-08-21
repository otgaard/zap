/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <graphics2/quad.hpp>

#include "apps/application.hpp"
#include "apps/graphic_types.hpp"

#include <rasteriser/canvas.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::graphics2;
using namespace zap::rasteriser;

template <typename Fnc> void repeat_fnc(size_t count, Fnc fnc) { for(size_t i = 0; i != count; ++i) fnc(); }

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
    quad img_;
    canvas canvas_;
    float angle_;
};

void zap_example::initialise() {
    img_.initialise();
    angle_ = 0.f;
}

void zap_example::on_resize(int width, int height) {
    application::on_resize(width, height);

    img_.resize(width, height);
    canvas_.resize(width, height);
    canvas_.clear();

    canvas_.pen_colour(colour::black8);

    transform3f T;
    T.translate(vec2f(width*.5f,height*.5f));
    T.uniform_scale(100.f);
    T.rotate(make_rotation(angle_));

    vec2f vertices[3] = {
            vec2f(-1.f,-1.f),
            vec2f(+1.f,-1.f),
            vec2f( 0.f,+1.f)
    };

    auto start = T.ptransform(vertices[0]);
    for(int i = 0; i != 4; ++i) {
        auto lP = T.ptransform(vertices[i%3]);
        canvas_.line(start.x, start.y, lP.x, lP.y);
        start = lP;
    }

    angle_ += 0.01f;

    texture tex;
    tex.allocate();
    tex.initialise(canvas_.width(), canvas_.height(), canvas_.buffer(), false);

    img_.set_texture(std::move(tex));
}

void zap_example::on_mousemove(double x, double y) {
}

void zap_example::on_mousewheel(double xinc, double yinc) {
}

void zap_example::update(double t, float dt) {
    img_.update(t,dt);
    on_resize(sc_width_, sc_height_);
}

void zap_example::draw() {
    img_.draw();
}

void zap_example::shutdown() {
}

int main(int argc, char* argv[]) {
    zap_example app;
    return app.run();
}

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
    angle_ = 0.f;
    img_.initialise();

    canvas_.resize(1280,768);
    canvas_.clear();

    img_.get_texture().allocate();
    img_.get_texture().initialise(1280,768,canvas_.buffer());
}

void zap_example::on_resize(int width, int height) {
    application::on_resize(width, height);

    if(canvas_.width() != width || canvas_.height() != height) {
        img_.resize(width, height);
        canvas_.resize(width, height);
        canvas_.clear();
        img_.get_texture().initialise(width, height, canvas_.buffer());
    }

    canvas_.map();
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
        auto wP = T.ptransform(vertices[i%3]);
        canvas_.line(start.x, start.y, wP.x, wP.y);
        start = wP;
    }

    canvas_.unmap();
    canvas_.update(img_.get_texture());

    angle_ += 0.01f;
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

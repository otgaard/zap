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
};

void zap_example::initialise() {
    img_.initialise();
}

void zap_example::on_resize(int width, int height) {
    LOG("RESIZE EVENT:", width, height);

    img_.resize(width, height);

    canvas_.resize(width, height);
    canvas_.clear();

    canvas_.pen_colour(colour::black8);

    vec2f disc1(width/4.f, height/2.f);
    vec2f disc2(width*(3/4.f), height/2.f);

    const size_t total = 40;
    const float inv_i = 1.f/total;
    for(int i = 0; i <= total; ++i) {
        float theta = i*inv_i*PI/2;
        float ct = std::cos(theta), st = std::sin(theta);
        canvas_.line3(disc1.x - 300*ct, disc1.y - 300*st, disc1.x + 300*ct, disc1.y + 300*st);
        canvas_.line3(disc1.x + 300*ct, disc1.y + 300*st, disc1.x - 300*ct, disc1.y - 300*st);
        canvas_.line3(disc1.x - 300*ct, disc1.y + 300*st, disc1.x + 300*ct, disc1.y - 300*st);
        canvas_.line3(disc1.x + 300*ct, disc1.y - 300*st, disc1.x - 300*ct, disc1.y + 300*st);

        canvas_.line3(disc2.x, disc2.y, disc2.x + 300*ct, disc2.y + 300*st);
        canvas_.line3(disc2.x, disc2.y, disc2.x - 300*ct, disc2.y - 300*st);
        canvas_.line3(disc2.x, disc2.y, disc2.x + 300*ct, disc2.y - 300*st);
        canvas_.line3(disc2.x, disc2.y, disc2.x - 300*ct, disc2.y + 300*st);

    }

    canvas_.pen_colour(colour::red8);
    canvas_.line3(disc1.x - 300, disc1.y, disc1.x + 300, disc1.y);
    canvas_.line3(disc1.x, disc1.y - 300, disc1.x, disc1.y + 300);

    canvas_.pen_colour(colour::blue8);
    canvas_.line3(disc2.x - 300, disc2.y, disc2.x + 300, disc2.y);
    canvas_.line3(disc2.x, disc2.y - 300, disc2.x, disc2.y + 300);

    //canvas_.line3(500,500,100,100);

    //canvas_.line3(10, 201, width-10, 199);
    //canvas_.line3(width-10, 201, 10, 203);

    /*
    maths::timer t(true);
    for(int i = 0; i != 1000000; ++i) {
        canvas_.pen_colour(vec3b(rand()%255, rand()%255, rand()%255));
        canvas_.line2(10, rand()%(height-20)+10, width - 10, rand()%(height-20)+10);
    }
    auto d = t.getf();
    t.stop();
    LOG(d, "seconds");

    t.start();
    for(int i = 0; i != 1000000; ++i) {
        canvas_.pen_colour(vec3b(rand()%255, rand()%255, rand()%255));
        canvas_.line3(10, rand()%(height-20)+10, width - 10, rand()%(height-20)+10);
    }

    d = t.getf();
    t.stop();
    LOG(d, "seconds");

    t.start();
    for(int i = 0; i != 1000000; ++i) {
        canvas_.pen_colour(vec3b(rand()%255, rand()%255, rand()%255));
        canvas_.line1(10, rand()%(height-20)+10, width - 10, rand()%(height-20)+10);
    }

    d = t.getf();
    t.stop();
    LOG(d, "seconds");
    */
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

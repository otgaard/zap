/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include <graphics2/quad.hpp>
#include "apps/application.hpp"
#include "apps/graphic_types.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::graphics2;

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
};

void zap_example::initialise() {
    img_.initialise();

    on_resize(1280,768);
}

void zap_example::on_resize(int width, int height) {
    application::on_resize(width, height);
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

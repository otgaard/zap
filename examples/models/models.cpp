//
// Created by Darren Otgaar on 2017/04/08.
//

#include <maths/io.hpp>
#include <tools/log.hpp>
#include <renderer/camera.hpp>
#include "host/GLFW/application.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::renderer;

class models : public application {
public:
    models() : application("models", 1280, 768, false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    camera cam_;
};

bool models::initialise() {
    cam_.viewport(0, 0, 1280, 768);
    cam_.frustum(45.f, 1280.f/768.f, .5f, 10.f);
    cam_.orthogonolise(vec3f(0,0,-1));
    return true;
}

void models::on_resize(int width, int height) {
}

void models::on_mousemove(double x, double y) {
}

void models::on_mousewheel(double xinc, double yinc) {
}

void models::update(double t, float dt) {
}

void models::draw() {
}

void models::shutdown() {
}

int main(int argc, char* argv[]) {
    models app;
    return app.run();
}

/* Created by Darren Otgaar on 2016/10/29 */

#include <maths/io.hpp>
#include <tools/log.hpp>
#include <engine/texture.hpp>
#include "host/GLFW/application.hpp"
#include "effects/particle_engine.hpp"
#include "renderer/camera.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::effects;
using namespace zap::renderer;

class part_engine : public application {
public:
    part_engine() : application("particle_engine", 1280, 768, false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;
    void on_mousemove(double x, double y) override final;
    void on_mousewheel(double xinc, double yinc) override final;

protected:
    particle_engine engine_;
    camera cam_;
};

bool part_engine::initialise() {
    if(!engine_.initialise()) {
        LOG_ERR("Failed to initialise the particle engine.  Terminating.");
        return false;
    }

    cam_.viewport(0, 0, 1280, 768);
    cam_.frustum(45.f, 1280.f/768.f, .5f, 10.f);

    vec3f starting_pos = vec3f(0.f, 2.f, 5.f);
    cam_.orthogonolise(normalise(-starting_pos));
    cam_.world_pos(starting_pos);

    return true;
}

void part_engine::on_resize(int width, int height) {
}

void part_engine::on_mousemove(double x, double y) {
}

void part_engine::on_mousewheel(double xinc, double yinc) {
}

void part_engine::update(double t, float dt) {
}

void part_engine::draw() {
    engine_.draw(cam_);
}

void part_engine::shutdown() {
}

int main(int argc, char* argv[]) {
    part_engine app;
    return app.run();
}

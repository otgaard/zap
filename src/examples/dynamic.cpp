//
// Created by Darren Otgaar on 2017/08/19.
//

#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <host/GLFW/application.hpp>
#include <engine/vertex_buffer.hpp>

using namespace zap;
using namespace zap::core;
using namespace zap::maths;
using namespace zap::engine;


class dynamic_app : public application {
public:
    dynamic_app() : application{"dynamic_app", 1024, 768} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

private:

};

bool dynamic_app::initialise() {
    return true;
}

void dynamic_app::update(double t, float dt) {
}

void dynamic_app::draw() {
}

void dynamic_app::shutdown() {
}

void dynamic_app::on_resize(int width, int height) {
}

int main(int argc, char* argv[]) {
    dynamic_app app{};
    return app.run();
}

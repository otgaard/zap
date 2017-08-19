//
// Created by Darren Otgaar on 2017/08/19.
//

#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <maths/io.hpp>
#include <host/GLFW/application.hpp>
#include <graphics2/quad.hpp>

class scene_graph_app : public application {
public:
    scene_graph_app() : application{"scene_graph", 1024, 768, false} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

private:
    zap::graphics::quad quad_;
};

bool scene_graph_app::initialise() {
    if(!quad_.initialise()) {
        LOG_ERR("Failed to initialise quad.");
        return false;
    }

    return true;
}

void scene_graph_app::update(double t, float dt) {
}

void scene_graph_app::draw() {
    quad_.draw();
}

void scene_graph_app::shutdown() {
}

void scene_graph_app::on_resize(int width, int height) {
    quad_.resize(width, height);
}

int main(int argc, char* argv[]) {
    scene_graph_app app{};
    return app.run();
}
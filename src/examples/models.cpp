//
// Created by otgaard on 2018/02/25.
//

#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <host/GLFW/application.hpp>
#include <graphics/graphics2/quad.hpp>
#include <graphics/loader/obj_loader.hpp>

const char* const MODEL_PATH = "C://Development//zap//assets//models//bunny.obj";

class models_app : public application {
public:
    models_app() : application{"models_app", 2*1024, 2*768} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

private:
    std::unique_ptr<zap::graphics::quad> quad_;
    std::unique_ptr<zap::graphics::mesh_p3n3t2_trii_t> mesh_;
};

bool models_app::initialise() {
    quad_ = std::make_unique<zap::graphics::quad>();
    if(!quad_->initialise()) {
        LOG_ERR("Failed to initialise quad.");
        return false;
    }

    mesh_ = zap::graphics::obj_loader::load(MODEL_PATH);
    if(!mesh_) {
        LOG_ERR("Failed to load Stanford Bunny Mesh");
        return false;
    }

    return true;
}

void models_app::update(double t, float dt) {
}

void models_app::draw() {
    quad_->draw();
}

void models_app::shutdown() {
    quad_.reset(nullptr);
}

void models_app::on_resize(int width, int height) {
    quad_->resize(width, height);
}

int main(int argc, char* argv[]) {
    models_app app{};
    return app.run();
}

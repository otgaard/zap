//
// Created by Darren Otgaar on 2017/08/19.
//

#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <maths/io.hpp>
#include <host/GLFW/application.hpp>
#include <graphics2/quad.hpp>
#include <loader/image_loader.hpp>

const char* const TEXTURE_PATH = "/Users/otgaard/Development/zap/assets/raxip.jpg";

class template_app : public application {
public:
    template_app() : application{"template_app", 1024, 768, false} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

private:
    zap::graphics::quad quad_;
};

bool template_app::initialise() {
    if(!quad_.initialise()) {
        LOG_ERR("Failed to initialise quad.");
        return false;
    }

    auto tex = zap::loader::load_texture2D<zap::engine::rgba8888_t>(TEXTURE_PATH, false, true);
    if(tex.is_allocated()) quad_.set_texture(std::move(tex));

    LOG("Loaded:", TEXTURE_PATH, tex.width(), tex.height());
    resize(tex.width(), tex.height());

    return true;
}

void template_app::update(double t, float dt) {
}

void template_app::draw() {
    quad_.draw();
}

void template_app::shutdown() {
}

void template_app::on_resize(int width, int height) {
    quad_.resize(width, height);
}

int main(int argc, char* argv[]) {
    template_app app{};
    return app.run();
}

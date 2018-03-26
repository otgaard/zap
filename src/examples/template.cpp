//
// Created by Darren Otgaar on 2017/08/19.
//

#include <tools/log.hpp>
#include <maths/algebra.hpp>
#include <host/GLFW/application.hpp>
#include <graphics/graphics2/quad.hpp>
#include <graphics/loader/image_loader.hpp>

#if defined(_WIN32)
const char* const TEXTURE_PATH = "C://Development/zap/assets/raxip.jpg";
#elif defined(__APPLE__)
const char* const TEXTURE_PATH = "/Users/otgaard/Development/zap/assets/raxip.jpg";
#endif

class template_app : public application {
public:
    template_app() : application{"template_app", 1024, 768} { }

    bool initialise() final;
    void update(double t, float dt) final;
    void draw() final;
    void shutdown() final;

    void on_resize(int width, int height) final;

private:
    std::unique_ptr<zap::graphics::quad> quad_;
};

bool template_app::initialise() {
    LOG("Here");
    quad_ = std::make_unique<zap::graphics::quad>();
    if(!quad_->initialise()) {
        LOG_ERR("Failed to initialise quad.");
        return false;
    }

    LOG("NOW HERE");
    auto tex = zap::loader::load_texture2D<zap::engine::rgb888_t>(TEXTURE_PATH, false, true);
    if(tex.is_allocated()) {
        LOG("Loaded:", TEXTURE_PATH, tex.width(), tex.height());
        resize(tex.width(), tex.height());
        quad_->set_texture(std::move(tex));
    }

    LOG("RETURNING");
    return true;
}

void template_app::update(double t, float dt) {
}

void template_app::draw() {
    quad_->draw();
}

void template_app::shutdown() {
    quad_.reset(nullptr);
}

void template_app::on_resize(int width, int height) {
    quad_->resize(width, height);
}

int main(int argc, char* argv[]) {
    template_app app{};
    return app.run();
}

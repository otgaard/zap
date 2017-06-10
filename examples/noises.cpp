//
// Created by Darren Otgaar on 2017/06/10.
//

/**
 * A test platform for implementing noise generators on the CPU/GPU (matching) and implementing/cleaning up the texture,
 * pixel_buffer, framebuffer, and readback functionality in the engine.
 *
 * 1) Value, Perlin, Simplex, and Worley
 * 2) Analytic derivatives where possible (evaluated otherwise)
 * 3) Matching implementation between CPU/GPU
 * 4) Fast readback from GPU (Pixel Buffers)
 */

#include <tools/log.hpp>
#include <maths/io.hpp>
#include <graphics2/quad.hpp>
#include <host/GLFW/application.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::graphics;
using namespace zap::renderer;

class noises : public application {
public:
    noises() : application("noises", 1280, 768, false) { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;

protected:
    quad quad_;
};

bool noises::initialise() {
    if(!quad_.initialise()) {
        LOG_ERR("Failed to initialise quad.");
        return false;
    }

    return true;
}

void noises::on_resize(int width, int height) {
    quad_.resize(width, height);
}

void noises::update(double t, float dt) {
}

void noises::draw() {
    quad_.draw();
}

void noises::shutdown() {
}

int main(int argc, char* argv[]) {
    noises app;
    return app.run();
}

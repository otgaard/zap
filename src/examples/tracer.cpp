/* Created by Darren Otgaar on 2016/08/07. http://www.github.com/otgaard/zap */

#include <maths/io.hpp>
#include <tools/log.hpp>
#include "host/GLFW/application.hpp"
#include "graphics/graphics2/quad.hpp"
#include "raytracer/raytracer.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::graphics;

class tracer : public application {
public:
    tracer() : application{"tracer", 800, 600} { }
    virtual ~tracer() { }

    bool initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;

protected:
    quad image;
};

bool tracer::initialise() {
    image.initialise();

    auto tex = texture();
    tex.allocate();

    raytracer rndr;
    auto pixels = rndr.render(1280, 768);
    tex.initialise(1280, 768, pixels, false);
    image.set_texture(std::move(tex));
    return true;
}

void tracer::update(double t, float dt) {
    //on_resize(sc_width_, sc_height_);
}

void tracer::draw() {
    image.draw();
}

void tracer::shutdown() {
    image.release();
}

void tracer::on_resize(int width, int height) {
    application::on_resize(width,height);
    image.resize(width, height);

    auto tex = texture();
    tex.allocate();
    raytracer rndr;
    auto pixels = rndr.render(width, height);
    tex.initialise(width, height, pixels, false);
    image.set_texture(std::move(tex));
}

int main(int argc, char* argv[]) {
    tracer app;
    return app.run();
}

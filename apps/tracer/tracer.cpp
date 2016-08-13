/* Created by Darren Otgaar on 2016/08/07. http://www.github.com/otgaard/zap */

#define LOGGING_ENABLED
#include <maths/io.hpp>
#include <tools/log.hpp>
#include "apps/application.hpp"
#include "graphics2/quad.hpp"
#include "renderer/camera.hpp"

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::graphics2;

class tracer : public application {
public:
    tracer() : application("tracer", 1280, 768, false) { }
    virtual ~tracer() { }

    void initialise() override final;
    void update(double t, float dt) override final;
    void draw() override final;
    void shutdown() override final;

    void on_resize(int width, int height) override final;

protected:
    transform3f world;
    quad image;
};

#include "generators/noise/value_noise.hpp"

void tracer::initialise() {
    image.initialise();

    auto tex = texture();
    tex.allocate();

    generators::noise noise;
    noise.initialise();

    const float inv_w = 32.f/1024;
    std::vector<rgb888_t> buffer(1024*1024);
    for(int r = 0; r != 1024; ++r) {
        for(int c = 0; c != 1024; ++c) {
            float x = c*inv_w; float y = r*inv_w;
            auto h = scale_bias(noise.fractal<generators::value_noise<float>>(4, 0.5f, 2.f, x, y, 0.f), 0.5f, .5f);
            buffer[r*1024+c].set3(255*h, 255*h, 255*h);
        }
    }

    tex.initialise(1024,1024,buffer,true);
    image.set_texture(std::move(tex));
}

void tracer::update(double t, float dt) {

}

void tracer::draw() {
    image.draw();
}

void tracer::shutdown() {

}

void tracer::on_resize(int width, int height) {
    image.resize(width, height);
}

int main(int argc, char* argv[]) {
    tracer app;
    return app.run();
}

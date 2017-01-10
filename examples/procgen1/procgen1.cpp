//
// Created by Darren Otgaar on 2017/01/10.
//

#include <engine/pixmap.hpp>
#include <engine/texture.hpp>
#include <graphics2/quad.hpp>
#include <maths/rand_lcg.hpp>
#include <host/GLFW/application.hpp>
#include <generators/noise/perlin.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::engine;
using namespace zap::graphics;
using namespace zap::generators;

class procgen1 : public application {
public:
    procgen1() : application("procgen1", 1024, 768, false) { }
    virtual ~procgen1() { }

    bool initialise() override;
    void update(double t, float dt) override;
    void draw() override;
    void shutdown() override;

    void on_resize(int width, int height) override;

private:
    quad quad_;
    rand_lcg rand_;
    texture tex_;
};

bool procgen1::initialise() {
    quad_.initialise();

    noise::initialise();

    // Build a star field
    pixmap<rgb888_t> pixbuf(1024, 768);
    for(auto r = 0; r != 768; ++r) {
        for(auto c = 0; c != 1024; ++c) {
            vec3b colour;

            auto intensity = clamp<uint8_t>(rand_.rand()%150, 100, 150);
            if(rand_.rand() % 2000 == 0) colour.set(intensity, intensity, 255);
            else                         colour.set(0,0,0);

            // Add some "gas"
            auto h = noise::turbulence<perlin<float>>(4, 1.f, .5f, 4.f*r/768.f, 4.f*c/1024.f);
            colour.set(colour.x, colour.y, clamp(colour.z + uint8_t(h*255), 0, 255));

            h = .4f*noise::turbulence<perlin<float>>(4, 1.f, .5f, 4.f*(r+2000)/768.f, 4.f*(c+2000)/1024.f);
            colour.set(clamp(colour.x  + uint8_t(h*255), 0, 255), colour.y, colour.z);

            pixbuf(c,r).set3(colour);
        }
    }

    tex_.allocate();
    tex_.initialise(1024, 768, pixbuf.buffer(), false);
    quad_.set_texture(std::move(tex_));

    return true;
}

void procgen1::update(double t, float dt) {
    quad_.update(t, dt);
}

void procgen1::draw() {
    quad_.draw();
}

void procgen1::shutdown() {
    application::shutdown();
}

void procgen1::on_resize(int width, int height) {
    application::on_resize(width, height);
}

int main(int argc, char* argv[]) {
    procgen1 app;
    app.run();
    return 0;
}

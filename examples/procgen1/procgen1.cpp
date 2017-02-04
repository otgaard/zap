//
// Created by Darren Otgaar on 2017/01/10.
//

#include "generators.hpp"

#include <maths/io.hpp>
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

    pixmap<rgba8888_t> generate(int width, int height) const;
};

bool procgen1::initialise() {
    quad_.initialise();

    noise::initialise();

    // Build a star field
    pixmap<rgba8888_t> pixbuf = generate(1024, 768);

    texture tex;
    tex.allocate();
    tex.initialise(1024, 768, pixbuf.buffer(), false);
    quad_.set_texture(std::move(tex));

    return true;
}

vec2f exp_spiral(float phi, float curvature, float offset) {
    float rad = std::exp(curvature*(phi));
    return polar_to_cartesian(vec2f(rad, phi+offset));
}

vec2f log_spiral(float phi, float A, float B, float N, float offset) {
    float rad = A / std::log(B * std::tan(phi/2*N));
    return polar_to_cartesian(vec2f(rad, phi+offset));
}

pixmap<rgba8888_t> procgen1::generate(int width, int height) const {
    const float inv_width = 1.f/width, inv_height = 1.f/height;

    pixmap<rgba8888_t> pixbuf(width, height);

    vec4b background(0,0,0,255);

    for(int i = 0; i != pixbuf.size(); ++i) pixbuf[i].set4(background);

    std::vector<pixmap<rgba8888_t>> stars = {
            make_radial(4, 4),
            make_radial(6, 6),
            make_radial(10, 10),
            make_radial(12, 12),
            make_radial(14, 14)
    };

    for(int i = 0; i != 200; ++i) { // Stars
        auto star = rand_.rand()%stars.size();

        auto f = .15f + rand_.random()*.85f;
        auto c = vec3f(.5f + .5f*rand_.random(), .5f + .5f*rand_.random(), .5f + .5f*rand_.random());
        stars[star].transform(0, 0, stars[star].width(), stars[star].height(), [f, c](auto& dest) {
            auto v = dest.template get4v<float>()/255.f;
            auto col = c * v.w * f * 255.f;
            dest.set3(col);
            return dest;
        });

        pixbuf.blend(rand_.rand() % width, rand_.rand() % height, stars[star], [](auto& dest, auto& src) {
            auto d = dest.template get4v<float>()/255.f, s = src.template get4v<float>()/255.f;
            rgba8888_t pix(dest);
            pix.set3(255.f*(d*(1.f - s.w) + s*(s.w)).xyz());
            return pix;
        });
    }

    for(auto r = 0; r != height; ++r) {
        for(auto c = 0; c != width; ++c) {
            vec4b colour = pixbuf(c,r).get4v<byte>();

            // Add some "gas"
            auto h = noise::turbulence<perlin<float>>(8, 1.f, .5f, 4.f * r * inv_height, 4.f * c * inv_width);
            colour.set(colour.x, colour.y, clamp(colour.z + uint8_t(h*255), 0, 255), 255);

            h = .4f * noise::turbulence<perlin<float>>(8, 1.f, .5f, 10.f * (r + 2000) * inv_height, 10.f * (c + 2000) * inv_width);
            colour.set(clamp(colour.x  + uint8_t(h*255), 0, 255), colour.y, colour.z, colour.w);

            pixbuf(c,r).set4(colour);
        }
    }
    return pixbuf;
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
    auto pixbuf = generate(width, height);
    texture tex;
    tex.allocate();
    tex.initialise(width, height, pixbuf.buffer(), false);
    quad_.resize(width, height);
    quad_.set_texture(std::move(tex));
}

int main(int argc, char* argv[]) {
    procgen1 app;
    app.run();
    return 0;
}

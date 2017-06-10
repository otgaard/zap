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
#include <tools/threadpool.hpp>
#include <generators/noise/noise.hpp>
#include <tuple>

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
    threadpool pool_;
    std::vector<std::future<bool>> completion_tokens_;
    pixmap<rgb888_t> image_;
};

bool noises::initialise() {
    if(!quad_.initialise()) {
        LOG_ERR("Failed to initialise quad.");
        return false;
    }

    if(!pool_.initialise(2)) {
        LOG_ERR("Threadpool failed to initialise");
        return false;
    }

    return true;
}

void noises::on_resize(int width, int height) {
    quad_.resize(width, height);
    image_.resize(width, height);

    auto fnc = [width](pixmap<rgb888_t>& image, int c, int r, int w, int h)->bool {
        const byte color = byte(c == 0 ? 0 : 255);
        for(int y = r, rend = r + h; y != rend; ++y) {
            for(int x = c, cend = c + w; x != cend; ++x) {
                image(x,y).set3(color, 255-color, 0);
            }
        }

        return true;
    };

    completion_tokens_.emplace_back(pool_.run_function(fnc, std::ref(image_), 0, 0, width/2, height));
    completion_tokens_.emplace_back(pool_.run_function(fnc, std::ref(image_), width/2, 0, width/2 + (width%2), height));
}

void noises::update(double t, float dt) {
    if(!completion_tokens_.empty()) {
        bool finished = true;
        for(auto& c : completion_tokens_)
            finished &= c.wait_for(std::chrono::milliseconds(2)) == std::future_status::ready;

        if(finished) {
            LOG("FINISHED");

            completion_tokens_.clear();
            texture tex{};
            tex.allocate();
            tex.initialise(image_, false);
            quad_.set_texture(std::move(tex));
        }
    }
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

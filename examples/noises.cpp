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
#include <generators/generator.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::graphics;
using namespace zap::renderer;

class noises : public application {
public:
    noises() : application("noises", 1200, 1200, false) { }

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
    generator gen_;
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

    if(!gen_.initialise()) {
        LOG_ERR("Error initialising texture generator");
        return false;
    }

    return true;
}

//#define POOL

void noises::on_resize(int width, int height) {
    quad_.resize(width, height);
    image_.resize(width, height);

#if defined(POOL)
    auto fnc = [width](pixmap<rgb888_t>& image, int c, int r, int w, int h)->bool {
        const byte colourA = byte(c == 0 ? 0 : 255);
        const byte colourB = byte(r == 0 ? 0 : 255);
        for(int y = r, rend = r + h; y != rend; ++y) {
            for(int x = c, cend = c + w; x != cend; ++x) {
                image(x,y).set3(colourA, 255 - colourA, colourB);
            }
        }

        return true;
    };

    completion_tokens_.emplace_back(pool_.run_function(fnc, std::ref(image_), 0, 0, width/2, height/2));
    completion_tokens_.emplace_back(pool_.run_function(fnc, std::ref(image_), 0, height/2, width/2, height/2 + (height%2)));
    completion_tokens_.emplace_back(pool_.run_function(fnc, std::ref(image_), width/2, 0, width/2 + (width%2), height/2));
    completion_tokens_.emplace_back(pool_.run_function(fnc, std::ref(image_), width/2, height/2, width/2 + (width%2), height/2 + (height%2)));
#else   // !defined(POOL)
    render_task task{width, height};
    task.scale.set(50.f, 50.f);
    auto start = std::chrono::high_resolution_clock::now();
    auto img = gen_.render(task, generator::gen_method::SIMD).get();
    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration<float>(end - start).count();
    UNUSED(dur);
    LOG("Time:", dur);
    pixmap<rgb888_t> image{width, height};
    for(auto i = 0; i != img.size(); ++i) {
        byte b = (byte)(255.f*(.5f + .5f*img(i)));
        image(i).set3(b, b, b);
    }

    texture tex{};
    tex.allocate();
    tex.initialise(image, false);
    quad_.set_texture(std::move(tex));
#endif  // !defined(POOL)
}

void noises::update(double t, float dt) {
#if defined(POOL)
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
#endif
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

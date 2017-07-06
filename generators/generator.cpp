/* Created by Darren Otgaar on 2017/06/11. http://www.github.com/otgaard/zap */
#include <maths/rand_lcg.hpp>
#include <tools/threadpool.hpp>
#include <random>
#include "generator.hpp"

using namespace zap;
using namespace engine;

const int RND_TBL = 256;
const int RND_MASK = 255;

struct generator::state_t {
    threadpool* pool_ptr;

    bool initialised;
    byte prn_table[RND_TBL];
    float grad1[RND_TBL];
    maths::vec2f grad2[RND_TBL];
    state_t() : pool_ptr(nullptr), initialised(false) { }

    byte perm(int x) { return prn_table[x & RND_MASK]; }
    byte perm(int x, int y) { return perm(x + perm(y)); }
    byte perm(int x, int y, int z) { return perm(x + perm(y + perm(z))); }
    byte perm(int x, int y, int z, int w) { return perm(x + perm(y + perm(z + perm(w)))); }
};

generator::generator() : state_(new state_t()), s(*state_.get()) {
    UNUSED(RND_MASK);
}

zap::generator::~generator() = default;

bool zap::generator::initialise(threadpool* pool_ptr, int pool_size, ulonglong seed) {
    maths::rand_lcg prn(seed);

    if(!pool_ptr) {
        s.pool_ptr = new threadpool{};
        if(!s.pool_ptr->initialise(pool_size)) {
            LOG_ERR("Failed to initialise threadpool.  Aborting.");
            return false;
        }
    } else {
        s.pool_ptr = pool_ptr;
    }

    for(int i = 0; i != RND_TBL; ++i) {
        s.prn_table[i] = byte(i);
        s.grad1[i] = prn.random_s();
        s.grad2[i] = maths::normalise(maths::vec2f{prn.random_s(), prn.random_s()});
    }

    std::minstd_rand rd((uint32_t)seed);
    std::mt19937 gen(rd());
    std::shuffle(s.prn_table, s.prn_table+RND_TBL, gen);

    s.initialised = true;
    return true;
}

std::future<generator::pixmap<float>> generator::render(const render_task& req, generator::gen_method method) {
    auto fnc = [this, method](const auto& r)->generator::pixmap<float>  {
        switch(method) {
            case gen_method::CPU: return render_cpu(r);
            case gen_method::SIMD: return render_simd(r);
            case gen_method::GPU: return render_gpu(r);
        }
    };

    return s.pool_ptr->run_function(fnc, std::cref(req));
}

pixmap<float> generator::render_cpu(const render_task& req) {
    pixmap<float> image{req.width, req.height};
    const float inv_x = req.scale.x/req.width;
    const float inv_y = req.scale.y/req.height;

    using maths::floor;

    for(int r = 0; r != req.height; ++r) {
        float y = inv_y * r;
        for(int c = 0; c != req.width; ++c) {
            float x = inv_x * c;
            int xi = floor(x), yi = floor(y);
            float dx = x - xi, dy = y - yi;
            image(c,r) = maths::bilinear(dx, dy, s.grad1[s.perm(xi, yi)],   s.grad1[s.perm(xi+1, yi)],
                                                 s.grad1[s.perm(xi, yi+1)], s.grad1[s.perm(xi+1, yi+1)]);
        }
    }

    return image;
}

pixmap<float> generator::render_simd(const render_task& req) {
    return zap::generator::pixmap<float>();
}

pixmap<float> generator::render_gpu(const render_task& req) {
    return zap::generator::pixmap<float>();
}

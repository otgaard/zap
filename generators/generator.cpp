/* Created by Darren Otgaar on 2017/06/11. http://www.github.com/otgaard/zap */
#include <maths/rand_lcg.hpp>
#include <tools/threadpool.hpp>
#include <random>
#include "generator.hpp"
#include <maths/simd.hpp>

using namespace zap;
using namespace maths;
using namespace engine;

const int RND_TBL = 256;
const int RND_MASK = 255;

const simd::veci RND_MASK_V = simd::load(0xFF);

struct generator::state_t {
    threadpool* pool_ptr;

    bool initialised;
    byte prn_table[RND_TBL];
#if defined(_WIN32)
    __declspec(align(16)) float grad1[RND_TBL];
    __declspec(align(16)) maths::vec2f grad2[RND_TBL];
#else
    float __attribute__((aligned(16))) grad1[RND_TBL];
    maths::vec2f __attribute__((aligned(16))) grad2[RND_TBL];
#endif
    state_t() : pool_ptr(nullptr), initialised(false) { }

    byte perm(int x) { return prn_table[x & RND_MASK]; }
    byte perm(int x, int y) { return perm(x + perm(y)); }
    byte perm(int x, int y, int z) { return perm(x + perm(y + perm(z))); }
    byte perm(int x, int y, int z, int w) { return perm(x + perm(y + perm(z + perm(w)))); }

    float grad_a(int x) {
        return grad1[perm(x)];
    }

    float grad_b(int x) {
        x = (x >> 13) ^ x;
        int xx = (x * (x * x * 19990303) + 1376312589) & 0x7ffffff;
        return 1.f - ((float)xx / 1073741824.f);
    }

    simd::veci VCALL grad_v(const simd::veci& seed, const simd::veci& x) {
        simd::veci hash = seed;
        hash = _mm_xor_si128(x, hash);
        //hash = _mm_mul_ep
        return x;
    }

    simd::veci VCALL perm_v(const simd::vecm32i& x) {
        simd::vecm32i idx;
        idx.v = _mm_castsi128_ps(_mm_and_si128(_mm_castps_si128(x.v), RND_MASK_V));
        return _mm_set_epi32(prn_table[idx.arr[0]], prn_table[idx.arr[1]], prn_table[idx.arr[2]], prn_table[idx.arr[3]]);
    }

    simd::veci VCALL perm_v(const simd::vecm32i& x, const simd::vecm32i& y) {
        simd::vecm32i tmp1;
        tmp1.v = _mm_add_epi32(_mm_castps_si128(x.v), _mm_castps_si128(perm_v(y)));
        return perm_v(tmp1);
    }

    simd::vecm32f VCALL grad_v(const simd::vecm32i& x, const simd::vecm32i& y) {
        simd::vecm32i idx;
        idx.v = _mm_castsi128_ps(perm_v(x, y));

        return simd::vecm32f{{
            grad1[idx.arr[0]],
            grad1[idx.arr[1]],
            grad1[idx.arr[2]],
            grad1[idx.arr[3]]
        }};
    }
};

generator::generator() : state_(new state_t()), s(*state_.get()) {
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

    LOG(LOG_BOLDGREEN, "Running Lookup vs Computation Tests");

    using clock = std::chrono::high_resolution_clock;
    using duration = std::chrono::duration<float>;

    auto start = clock::now();

    float total = -std::numeric_limits<float>::max();
    for(size_t i = 0; i != 10000000; ++i) {
        float v = s.grad_a(i);
        total += v;
    }

    auto end = clock::now();

    LOG(LOG_BOLDRED, "Time to look up value in table:", duration(end - start).count());

    start = clock::now();

    for(size_t i = 0; i != 10000000; ++i) {
        float v = s.grad_b(i);
        total += v;
    }

    end = clock::now();
    LOG(LOG_BOLDRED, "Time to look up value in table:", duration(end - start).count());

    LOG(total);

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
    LOG("CPU");
    pixmap<float> image{req.width, req.height};

    auto start = std::chrono::high_resolution_clock::now();

    const float inv_x = req.scale.x/req.width;
    const float inv_y = req.scale.y/req.height;

    using maths::floor;

    for(int outer = 0; outer != 1000; ++outer) {
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
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration<float>(end - start).count();
    LOG("CPU Total Time: ", dur);

    return image;
}

pixmap<float> generator::render_simd(const render_task& req) {
    LOG("SIMD");
    pixmap<float> image{req.width, req.height};

    auto start = std::chrono::high_resolution_clock::now();

    const float inv_x = req.scale.x/req.width;
    const float inv_y = req.scale.y/req.height;
    using namespace zap::maths::simd;

    const int stream_size = 4;
    const int blocks = req.width/stream_size;

    const vecm32f vseq = { 0.f, 1.f, 2.f, 3.f };
    const vecm32f vinc = { inv_x, inv_x, inv_x, inv_x };
    const vecm vsteps = _mm_mul_ps(vseq, vinc);

    set_round_down();

    // Do it 100 times

    for(int outer = 0; outer != 1000; ++outer) {
    for(int r = 0; r != req.height; ++r) {
        float vy = r * inv_y;
        int iy = floor(vy);
        vecm dy = load(vy - (float)iy);
        vecm32i iy_v = load(iy);
        vecm32i iyp1 = load(iy+1);
        for(int c = 0; c != blocks; ++c) {
            const int c_offset = c * stream_size;
            vecm vx = _mm_add_ps(_mm_mul_ps(load((float)c_offset), vinc.v), vsteps);
            vecm fx = floor_v(vx);
            vecm32i ix = _mm_castsi128_ps(convert_v(fx));
            vecm32i ixp1 = _mm_castsi128_ps(_mm_add_epi32(_mm_castps_si128(ix.v), veci_one));

            vecm P00 = s.grad_v(ix, iy_v);
            vecm P01 = s.grad_v(ixp1, iy_v);
            vecm P10 = s.grad_v(ix, iyp1);
            vecm P11 = s.grad_v(ixp1, iyp1);

            vecm dx = _mm_sub_ps(vx, fx);

            vecm32f res = bilinear_v(dx, dy, P00, P01, P10, P11);
            _mm_store_ps(image.data(c_offset, r), res);
        }
    }
    }

    set_round_default();

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration<float>(end - start).count();
    LOG("SIMD Total Time: ", dur);

    return image;
}

pixmap<float> generator::render_gpu(const render_task& req) {
    LOG("SIMD2");
    pixmap<float> image{req.width, req.height};

    auto start = std::chrono::high_resolution_clock::now();

    const float inv_x = req.scale.x/req.width;
    const float inv_y = req.scale.y/req.height;
    using namespace zap::maths::simd;

    const int stream_size = 4;
    const int blocks = req.width/stream_size;

    const vecm32f vseq = {{ 1.f, 2.f, 3.f, 4.f }};
    const vecm32f vinc = {{ inv_x, inv_x, inv_x, inv_x }};
    const vecm vsteps = _mm_mul_ps(vseq.v, vinc.v);

    set_round_down();

    // Do it 100 times

    for(int outer = 0; outer != 1000; ++outer) {
        for(int r = 0; r != req.height; ++r) {
            float vy = r * inv_y;
            int iy = floor(vy);
            int iyp1 = iy+1;
            float dy = vy - iy;
            vecm dv = load(dy);
            for(int c = 0; c != blocks; ++c) {
                int c_offset = c * stream_size;
                vecm vx = _mm_add_ps(_mm_mul_ps(load((float)c_offset), vinc.v), vsteps);
                vecm fx = ffloor_v(vx);
                vecm32i ix;
                ix.v = _mm_castsi128_ps(convert_v(fx));

                vecm32i ixp1;
                ixp1.v = _mm_castsi128_ps(_mm_add_epi32(ix.v, veci_one));

                vecm32f P00 = {{ s.grad1[s.perm(ix.arr[0], iy)], s.grad1[s.perm(ix.arr[1], iy)], s.grad1[s.perm(ix.arr[2], iy)], s.grad1[s.perm(ix.arr[3], iy)] }};
                vecm32f P01 = {{ s.grad1[s.perm(ixp1.arr[0],iy)], s.grad1[s.perm(ixp1.arr[1],iy)], s.grad1[s.perm(ixp1.arr[2],iy)], s.grad1[s.perm(ixp1.arr[3],iy)] }};
                vecm32f P10 = {{ s.grad1[s.perm(ix.arr[0],iyp1)], s.grad1[s.perm(ix.arr[1],iyp1)], s.grad1[s.perm(ix.arr[2],iyp1)], s.grad1[s.perm(ix.arr[3],iyp1)] }};
                vecm32f P11 = {{ s.grad1[s.perm(ixp1.arr[0],iyp1)], s.grad1[s.perm(ixp1.arr[1],iyp1)], s.grad1[s.perm(ixp1.arr[2],iyp1)], s.grad1[s.perm(ixp1.arr[3],iyp1)] }};

                vecm dx = _mm_sub_ps(vx, fx);

                vecm32f res;
                res.v = bilinear_v(dx, dv, P00.v, P01.v, P10.v, P11.v);
                for(int i = 0; i != 4; ++i) image((int)(c_offset+i), r) = res.arr[i];
            }
        }
    }

    set_round_default();

    auto end = std::chrono::high_resolution_clock::now();
    auto dur = std::chrono::duration<float>(end - start).count();
    LOG("SIMD Total Time: ", dur);

    return image;
}

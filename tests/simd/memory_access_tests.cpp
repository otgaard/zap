//
// Created by Darren Otgaar on 2017/07/09.
//

#include <maths/simd.hpp>
#include <cstring>
#include <chrono>
#include <memory>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <maths/rand_lcg.hpp>
#define LOGGING_ENABLED
#include <tools/log.hpp>
#include <engine/pixmap.hpp>
#include <engine/pixel_format.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::maths::simd;

using hrclock = std::chrono::high_resolution_clock;
using duration_t = std::chrono::duration<float>;

// Compiled to leave assembly memory_access_tests.s
using byte = unsigned char;
static const int RND_TBL = 256;
static const int RND_MASK = 255;
//static const int width = 1024;
//static const int height = 1024;
//static const int px_count = width * height;
//static const int iterator_count = 10000;
static const int prime = 57559;
static const int seed = 8395;
static const veci vprime = _mm_set_epi32(prime, prime, prime, prime);
static const veci vseed = _mm_set_epi32(seed, seed, seed, seed);

// SIMD constants

const veci32i seq_vi = { 0, 1, 2, 3 };
const vecm32f seq_vf = { 0.f, 1.f, 2.f, 3.f };
const veci32i zero_vi = _mm_setzero_si128();
const veci32i one_vi = { 1, 1, 1, 1 };
const vecm32f one_vf = { 1.f, 1.f, 1.f, 1.f };
const vecm32f float_vf = {1.f/2147483648.f, 1.f/2147483648.f, 1.f/2147483648.f, 1.f/2147483648.f};

struct state_t {
    bool initialised;
    byte prn_table[RND_TBL];
#if defined(_WIN32)
    __declspec(align(16)) float grad1[RND_TBL];
#else
    float __attribute__((aligned(16))) grad1[RND_TBL];
#endif
    state_t() : initialised(false) {
        maths::rand_lcg prn(1);

        for(int i = 0; i != RND_TBL; ++i) {
            prn_table[i] = byte(i);
            grad1[i] = prn.random_s();
        }

        std::minstd_rand rd((uint32_t)1);
        std::mt19937 gen(rd());
        std::shuffle(prn_table, prn_table+RND_TBL, gen);

        initialised = true;
    }

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

    vecm VCALL grad_g(const veci& x, const veci& y) {
        veci hash = _mm_xor_si128(x, _mm_xor_si128(y, vseed));
        hash = mul(mul(mul(hash, hash), vprime), hash);
        return _mm_mul_ps(float_vf, _mm_cvtepi32_ps(hash));
    }

    veci VCALL perm_v(const vecm32i& x) {
        vecm32i idx;
        idx.v = _mm_castsi128_ps(_mm_and_si128(_mm_castps_si128(x.v), _mm_castps_si128(vecm_rnd_mask)));
        return _mm_set_epi32(prn_table[idx.arr[0]], prn_table[idx.arr[1]], prn_table[idx.arr[2]], prn_table[idx.arr[3]]);
    }

    veci VCALL perm_v(const vecm32i& x, const vecm32i& y) {
        vecm32i tmp1;
        tmp1.v = _mm_castsi128_ps(_mm_add_epi32(_mm_castps_si128(x.v), perm_v(y)));
        return perm_v(tmp1);
    }

    vecm32f VCALL grad_v(const vecm32i& x, const vecm32i& y) {
        vecm32i idx;
        idx.v = _mm_castsi128_ps(perm_v(x, y));

        return vecm32f{{
            grad1[idx.arr[0]],
            grad1[idx.arr[1]],
            grad1[idx.arr[2]],
            grad1[idx.arr[3]]
        }};
    }

    veci VCALL perm_v(const veci& x) {
        VALIGN int ii[4];
        veci idx = _mm_and_si128(x, veci_rnd_mask);
        _mm_store_si128((__m128i*)ii, idx);
        // This is far faster than _mm_setr_epi32
        VALIGN int iii[4] = { prn_table[ii[0]], prn_table[ii[1]], prn_table[ii[2]], prn_table[ii[3]] };
        return _mm_load_si128((veci*)iii);
    }

    veci VCALL perm_v(const veci& x, const veci& y) {
        veci32i idx = _mm_add_epi32(x, perm_v(y));
        return perm_v(idx);
    }

    vecm VCALL grad_v(const veci& x, const veci& y) {
        veci idx = perm_v(x, y);
        VALIGN int ii[4];
        _mm_store_si128((veci*)ii, idx);
        VALIGN float ff[4] = { grad1[ii[0]], grad1[ii[1]], grad1[ii[2]], grad1[ii[3]] };
        return _mm_load_ps(ff);
    }
};

static state_t global_state;

float total(float* buffer_ptr, size_t count) {
    float result = 0.f;
    for(size_t i = 0; i != count; ++i)
        result += buffer_ptr[i];
    return result;
}

// Testing image generation

float lerp_c(float value, float A, float B) {
    return (1.f - value)*A + value*B;
}

void render_cpu(int width, int height, float* buffer_ptr) {
    const int px_count = width * height;
    memset(buffer_ptr, 0x00000000, sizeof(float)*px_count);

    const float inv_w = 1.f/(width-1);
    const float A = 0.f, B = 10.f;

    for(int r = 0; r != height; ++r) {
        const int offset = r*width;
        for(int c = 0; c != width; ++c) {
            buffer_ptr[offset + c] = lerp_c(c*inv_w, A, B);
        }
    }
}

inline vecm32f lerp_v(const vecm32f& value, const vecm32f& A, const vecm32f& B) {
    return _mm_add_ps(_mm_mul_ps(_mm_sub_ps(one_vf, value), A), _mm_mul_ps(value, B));
}

void render_simd(int width, int height, float* buffer_ptr) {
    const int px_count = width * height;
    memset(buffer_ptr, 0x00000000, sizeof(float)*px_count);

    const float inv_w = 1.f/(width-1);
    const vecm steps = _mm_mul_ps(seq_vf, load(inv_w));
    const vecm A = _mm_setzero_ps(), B = load(10.f);

    for(int r = 0; r != height; ++r) {
        const int offset = r*width;
        for(int c = 0; c < width; c += 4) {
            _mm_store_ps(buffer_ptr+offset+c, lerp_v(_mm_add_ps(load(c * inv_w), steps), A, B));
        }
    }
}

void render_noiseA(int width, int height, float* buffer_ptr) {
    const float inv_x = 1.f/width;
    const float inv_y = 1.f/height;

    using maths::floor;

    size_t px = 0;
    for(int r = 0; r != height; ++r) {
        float y = inv_y * r;
        for(int c = 0; c != width; ++c) {
            float x = inv_x * c;
            int xi = floor(x), yi = floor(y);
            float dx = x - xi, dy = y - yi;
            *(buffer_ptr+px++) = maths::bilinear(dx, dy,
                                                 global_state.grad1[global_state.perm(xi, yi)],
                                                 global_state.grad1[global_state.perm(xi+1, yi)],
                                                 global_state.grad1[global_state.perm(xi, yi+1)],
                                                 global_state.grad1[global_state.perm(xi+1, yi+1)]);
        }
    }
}

void render_noiseB(int width, int height, float* buffer_ptr) {
    const float inv_x = 1.f/width;
    const float inv_y = 1.f/height;
    using namespace zap::maths::simd;

    const int stream_size = 4;
    const int blocks = width/stream_size;

    const vecm32f vseq = { 0.f, 1.f, 2.f, 3.f };
    const vecm32f vinc = { inv_x, inv_x, inv_x, inv_x };
    const vecm vsteps = _mm_mul_ps(vseq, vinc);

    set_round_down();

    for(int r = 0; r != height; ++r) {
        float vy = r * inv_y;
        int iy = (int)floor(vy);
        vecm dy = load(vy - (float)iy);
        vecm32i iy_v = _mm_castsi128_ps(load(iy));
        vecm32i iyp1 = _mm_castsi128_ps(load(iy+1));
        for(int c = 0; c != blocks; ++c) {
            const int c_offset = c * stream_size;
            vecm vx = _mm_add_ps(_mm_mul_ps(load((float)c_offset), vinc.v), vsteps);
            vecm fx = floor_v(vx);
            vecm32i ix = _mm_castsi128_ps(convert_v(fx));
            vecm32i ixp1 = _mm_castsi128_ps(_mm_add_epi32(_mm_castps_si128(ix.v), veci_one));

            // This code is hugely inefficient in SIMD
            vecm P00 = global_state.grad_v(ix, iy_v);
            vecm P01 = global_state.grad_v(ixp1, iy_v);
            vecm P10 = global_state.grad_v(ix, iyp1);
            vecm P11 = global_state.grad_v(ixp1, iyp1);

            vecm dx = _mm_sub_ps(vx, fx);

            vecm32f res = bilinear_v(dx, dy, P00, P01, P10, P11);
            _mm_store_ps(buffer_ptr+(r*width+c_offset), res);
        }
    }

    set_round_default();
}

void render_noiseC(int width, int height, float* buffer_ptr) {
    const float inv_x = 1.f/width;
    const float inv_y = 1.f/height;
    using namespace zap::maths::simd;

    const int stream_size = 4;
    const int blocks = width/stream_size;

    const vecm32f vseq = { 0.f, 1.f, 2.f, 3.f };
    const vecm32f vinc = { inv_x, inv_x, inv_x, inv_x };
    const vecm vsteps = _mm_mul_ps(vseq, vinc);

    set_round_down();

    for(int r = 0; r != height; ++r) {
        float vy = r * inv_y;
        int iy = (int)floor(vy);
        vecm dy = load(vy - (float)iy);
        veci iy_v = load(iy);
        veci iyp1 = load(iy+1);

        for(int c = 0; c != blocks; ++c) {
            const int c_offset = c * stream_size;
            vecm vx = _mm_add_ps(load(c_offset * inv_x), vsteps);
            vecm fx = ffloor_v(vx);
            veci ix = convert_v(fx);
            veci ixp1 = _mm_add_epi32(ix, veci_one);

            // This code is hugely inefficient in SIMD
            vecm P00 = global_state.grad_v(ix, iy_v);
            vecm P01 = global_state.grad_v(ixp1, iy_v);
            vecm P10 = global_state.grad_v(ix, iyp1);
            vecm P11 = global_state.grad_v(ixp1, iyp1);

            vecm dx = _mm_sub_ps(vx, fx);

            vecm res = bilinear_v(dx, dy, P00, P01, P10, P11);
            _mm_store_ps(buffer_ptr+(r*width+c_offset), res);
        }
    }

    set_round_default();
}

void render_noiseD(int width, int height, float* buffer_ptr) {
    const float inv_x = 1.f/width;
    const float inv_y = 1.f/height;
    using namespace zap::maths::simd;

    const int stream_size = 4;
    const int blocks = width/stream_size;

    const vecm32f vseq = { 0.f, 1.f, 2.f, 3.f };
    const vecm32f vinc = { inv_x, inv_x, inv_x, inv_x };
    const vecm vsteps = _mm_mul_ps(vseq, vinc);

    set_round_down();

    for(int r = 0; r != height; ++r) {
        float vy = r * inv_y;
        int iy = (int)floor(vy);
        vecm dy = load(vy - (float)iy);
        veci iy_v = load(iy);
        veci iyp1 = load(iy+1);

        for(int c = 0; c != blocks; ++c) {
            const int c_offset = c * stream_size;
            vecm vx = _mm_add_ps(load(c_offset * inv_x), vsteps);
            vecm fx = ffloor_v(vx);
            veci ix = convert_v(fx);
            veci ixp1 = _mm_add_epi32(ix, veci_one);

            // This code is hugely inefficient in SIMD
            vecm P00 = global_state.grad_g(ix, iy_v);
            vecm P01 = global_state.grad_g(ixp1, iy_v);
            vecm P10 = global_state.grad_g(ix, iyp1);
            vecm P11 = global_state.grad_g(ixp1, iyp1);

            vecm dx = _mm_sub_ps(vx, fx);

            vecm res = bilinear_v(dx, dy, P00, P01, P10, P11);
            _mm_store_ps(buffer_ptr+(r*width+c_offset), res);
        }
    }

    set_round_default();
}

void render_noiseE(int width, int height, float* buffer_ptr) {
    const float inv_x = 1.f/width;
    const float inv_y = 1.f/height;
    using namespace zap::maths::simd;

    const int stream_size = 4;
    const int blocks = width/stream_size;

    const vecm32f vseq = { 0.f, 1.f, 2.f, 3.f };
    const vecm32f vinc = { inv_x, inv_x, inv_x, inv_x };
    const vecm vsteps = _mm_mul_ps(vseq, vinc);

    set_round_down();

    for(int r = 0; r != height; ++r) {
        float vy = r * inv_y;
        int iy = zap::maths::floor(vy);
        int iyp1 = iy + 1;
        vecm dy = load(vy - float(iy));
        int col_offset = r * width;
        for(int c = 0, offset = 0; c != blocks; ++c, col_offset += 4, offset += 4) {
            vecm vx = _mm_add_ps(load(offset * inv_x), vsteps);
            vecm fx = ffloor_v(vx);
            veci ix = convert_v(fx);
            veci ixp1 = _mm_add_epi32(ix, veci_one);
            vecm dx = _mm_sub_ps(vx, fx);

            int xi[4], xip1[4];
            _mm_store_si128((veci*)xi, ix);
            _mm_store_si128((veci*)xip1, ixp1);
            VALIGN float values[16];
            for(int i = 0; i != 4; ++i) {
                values[i] = global_state.grad1[global_state.perm(xi[i], iy)];
                values[4+i] = global_state.grad1[global_state.perm(xip1[i], iy)];
                values[8+i] = global_state.grad1[global_state.perm(xi[i], iyp1)];
                values[12+i] = global_state.grad1[global_state.perm(xip1[i], iyp1)];
            }

            vecm res = bilinear_v(dx, dy, _mm_load_ps(values), _mm_load_ps(values+4), _mm_load_ps(values+8), _mm_load_ps(values+12));
            _mm_store_ps(buffer_ptr+col_offset, res);
        }
    }

    set_round_default();
}

void test_noise_implementations(int iterations) {
    const char* fn_names[5] = { "render_noiseA", "render_noiseB", "render_noiseC", "render_noiseD", "render_noiseE" };
    void(*functions[5])(int,int,float*) = { &render_noiseA, &render_noiseB, &render_noiseC, &render_noiseD, &render_noiseE };

    const int width = 1024, height = 1024;
    const int dims = width*height;
    auto start = hrclock::now(), end = hrclock::now();
    std::vector<float> buffer(dims);
    std::vector<float> timings(iterations);

    int fn = 0;
    for(auto& function : functions) {
        // Warmup
        for(int iter = 0, count = iterations/10; iter != count; ++iter) function(width, height, buffer.data());

        for(int iter = 0; iter != iterations; ++iter) {
            start = hrclock::now();
            function(width, height, buffer.data());
            end = hrclock::now();
            timings[iter] = duration_t(end - start).count();
        }

        float min = std::numeric_limits<float>::max(), max = -min, avg = 0.f;
        for(auto& v : timings) {
            if(v < min) min = v;
            if(v > max) max = v;
            avg += v;
        }

        LOG(LOG_RED, "Time for", fn_names[fn++], "Total:", avg, "Avg:", avg/iterations, "Min:", min, "Max:", max);
        LOG(LOG_YELLOW, "Array vals:", buffer[0], buffer[1], buffer[2], buffer[3],
            buffer[4], buffer[5], buffer[6], buffer[7]);
    }
}

zap::engine::pixmap<zap::engine::rgb888_t> conversion_cpu(const zap::engine::pixmap<float>& img) {
    using namespace zap::engine;
    using pixmap = pixmap<rgb888_t>;
    pixmap ret{img.width(), img.height()};

    for(auto i = 0; i != img.size(); ++i) {
        byte px = (byte)(img[i] * 127.f + 127.f);
        ret[i].set(px);
    }

    return ret;
}

zap::engine::pixmap<zap::engine::rgb888_t> conversion_simd(const zap::engine::pixmap<float>& img) {
    using namespace zap::engine;
    using pixmap = pixmap<rgb888_t>;
    pixmap ret{img.width(), img.height()};

    const vecm vhalf = _mm_set_ps(127.f, 127.f, 127.f, 127.f);

    set_round_down();

    union {
        byte arr[16];    // 128 bits
        int32_t arri[4];
    };

    for(auto i = 0; i != img.size(); i += 4) {
        vecm val = _mm_load_ps(img.data(i));
        val = _mm_add_ps(_mm_mul_ps(val, vhalf), vhalf);
        veci vi = _mm_cvtps_epi32(val);
        vi = _mm_packus_epi32(vi, vi);
        vi = _mm_packus_epi16(vi, vi);
        *arri = _mm_cvtsi128_si32(vi);
        ret[i].set(arr[0]);
        ret[i+1].set(arr[1]);
        ret[i+2].set(arr[2]);
        ret[i+3].set(arr[3]);
    }

    set_round_default();

    return ret;
}

zap::engine::pixmap<zap::engine::rgb888_t> conversion_simdB(const zap::engine::pixmap<float>& img) {
    using namespace zap::engine;
    using pixmap = pixmap<rgb888_t>;
    pixmap ret{img.width(), img.height()};

    const vecm vhalf = _mm_set_ps(127.f, 127.f, 127.f, 127.f);

    set_round_down();

    byte arr[16];    // 128 bits
    for(auto i = 0; i != img.size(); i += 16) {
        vecm valA = _mm_load_ps(img.data(i));
        vecm valB = _mm_load_ps(img.data(i+4));
        valA = _mm_add_ps(_mm_mul_ps(valA, vhalf), vhalf);
        valB = _mm_add_ps(_mm_mul_ps(valB, vhalf), vhalf);

        veci viA = _mm_cvtps_epi32(valA);
        veci viB = _mm_cvtps_epi32(valB);
        viA = _mm_packs_epi32(viA, viB);

        valB = _mm_load_ps(img.data(i+8));
        vecm valC = _mm_load_ps(img.data(i+12));
        valB = _mm_add_ps(_mm_mul_ps(valB, vhalf), vhalf);
        valC = _mm_add_ps(_mm_mul_ps(valC, vhalf), vhalf);

        viB = _mm_cvtps_epi32(valB);
        veci viC = _mm_cvtps_epi32(valC);
        viB = _mm_packs_epi32(viB, viC);

        viC = _mm_packus_epi16(viA, viB);
        _mm_store_si128((veci*)arr, viC);

        for(int c = 0; c != 16; ++c) ret[c].set(arr[15-c]);
    }

    set_round_default();

    return ret;
}

void test_conversion_implementations(int iterations) {
    const char* fn_names[3] = { "conversion_cpu", "conversion_simd", "conversion_simdB" };
    zap::engine::pixmap<zap::engine::rgb888_t>(*functions[3])(const zap::engine::pixmap<float>&) = {
        &conversion_cpu, &conversion_simd, &conversion_simdB
    };

    const int width = 1024, height = 1024;
    const int dims = width*height;
    auto start = hrclock::now(), end = hrclock::now();
    zap::engine::pixmap<float> buffer(dims);
    std::vector<float> timings(iterations);

    render_noiseE(width, height, buffer.data());

    int fn = 0;
    for(auto& function : functions) {
        // Warmup
        for(int iter = 0, count = iterations/10; iter != count; ++iter) function(buffer);

        for(int iter = 0; iter != iterations; ++iter) {
            start = hrclock::now();
            function(buffer);
            end = hrclock::now();
            timings[iter] = duration_t(end - start).count();
        }

        float min = std::numeric_limits<float>::max(), max = -min, avg = 0.f;
        for(auto& v : timings) {
            if(v < min) min = v;
            if(v > max) max = v;
            avg += v;
        }

        auto output = function(buffer);

        LOG(LOG_RED, "Time for", fn_names[fn++], "Total:", avg, "Avg:", avg/iterations, "Min:", min, "Max:", max);
        LOG(LOG_YELLOW, "Array vals:", (int)output[0].get(0), (int)output[1].get(0), (int)output[2].get(0), (int)output[3].get(0),
            (int)output[4].get(0), (int)output[5].get(0), (int)output[6].get(0), (int)output[7].get(0));
        LOG(LOG_YELLOW, "Array vals:", (int)output[8].get(0), (int)output[9].get(0), (int)output[10].get(0), (int)output[11].get(0),
            (int)output[12].get(0), (int)output[13].get(0), (int)output[14].get(0), (int)output[15].get(0));
    }
}

int main(int argc, char* argv[]) {
    //test_noise_implementations(1000);
    test_conversion_implementations(10000);
    return 0;
}

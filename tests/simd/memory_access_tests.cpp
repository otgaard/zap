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
#include <maths/rand_lcg.hpp>

using namespace zap;
using namespace zap::maths;
using namespace zap::maths::simd;

using hrclock = std::chrono::high_resolution_clock;
using duration_t = std::chrono::duration<float>;

// Compiled to leave assembly memory_access_tests.s
using byte = unsigned char;
static const int RND_TBL = 256;
static const int RND_MASK = 255;
static const int width = 1024;
static const int height = 1024;
static const int px_count = width * height;
static const int iterator_count = 10000;

// SIMD constants

const veci32i seq_vi = { 0, 1, 2, 3 };
const vecm32f seq_vf = { 0.f, 1.f, 2.f, 3.f };
const veci32i zero_vi = _mm_setzero_si128();
const veci32i one_vi = { 1, 1, 1, 1 };
const vecm32f one_vf = { 1.f, 1.f, 1.f, 1.f };


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
        veci32i idx = _mm_and_si128(x, veci_rnd_mask);
        VALIGN int ii[4];
        ii[0] = prn_table[idx.arr[0]];
        ii[1] = prn_table[idx.arr[1]];
        ii[2] = prn_table[idx.arr[2]];
        ii[3] = prn_table[idx.arr[3]];
        return _mm_load_si128((veci*)ii);
    }

    veci VCALL perm_v(const veci& x, const veci& y) {
        veci32i idx = _mm_add_epi32(x, perm_v(y));
        return perm_v(idx);
    }

    vecm VCALL grad_v(const veci& x, const veci& y) {
        veci32i idx = perm_v(x, y);
        return vecm32f{{
                grad1[idx.arr[0]],
                grad1[idx.arr[1]],
                grad1[idx.arr[2]],
                grad1[idx.arr[3]]
        }};
    }
};

static state_t global_state;

float total(float* buffer_ptr, size_t count) {
    float result = 0.f;
    for(int i = 0; i != count; ++i)
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
            vecm vx = _mm_add_ps(load(c_offset * vinc.arr[0]), vsteps);
            vecm fx = ffloor_v(vx);
            veci ix = convert_v(fx);
            veci ixp1 = _mm_add_epi32(ix, veci_one);

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

veci testA() {
    const veci zeroi = _mm_set_epi32(10, 9, 8, 7);
    const veci seqi = _mm_set_epi32(4, 3, 2, 1);

    return _mm_add_epi32(zeroi, seqi);
}

veci32i testB() {
    const veci32i A = {7, 8, 9, 10};
    const veci32i B = {1, 2, 3, 4};

    return _mm_add_epi32(A, B);
}

void test_one() {
    testA();
    testB();

    // Note: Can't assume 16-byte alignment, but new always returns 16-byte aligned memory on 64-bit macOS.
    float* bufferA_ptr = new float[px_count];
    float* bufferB_ptr = new float[px_count];

    std::vector<float> cpu_times(iterator_count);
    std::vector<float> simd_times(iterator_count);

    auto start = hrclock::now(), end = hrclock::now();

    for(int i = 0; i != iterator_count; ++i) {
        start = hrclock::now();
        render_cpu(width, height, bufferA_ptr);
        end = hrclock::now();
        cpu_times[i] = duration_t(end - start).count();

        start = hrclock::now();
        render_simd(width, height, bufferB_ptr);
        end = hrclock::now();
        simd_times[i] = duration_t(end - start).count();
    }

    float min = std::numeric_limits<float>::max(), max = -min, avg = 0.f;
    for(auto& v : cpu_times) {
        if(v < min) min = v;
        if(v > max) max = v;
        avg += v;
    }

    std::cout << "Time measured for CPU, total: " << avg << ", avg: " << avg/iterator_count << ", min: " << min << ", max: " << max << std::endl;

    min = std::numeric_limits<float>::max(); max = -min; avg = 0.f;
    for(auto& v : simd_times) {
        if(v < min) min = v;
        if(v > max) max = v;
        avg += v;
    }

    std::cout << "Time measured for SIMD, total: " << avg << ", avg: " << avg/iterator_count << ", min: " << min << ", max: " << max << std::endl;

    for(int i = 0; i != 1024; ++i) {
        std::cout << bufferA_ptr[i] << ", ";
    }
    std::cout << std::endl;

    for(int i = 0; i != 1024; ++i) {
        std::cout << bufferB_ptr[i] << ", ";
    }
    std::cout << std::endl;

    delete[] bufferA_ptr;
    delete[] bufferB_ptr;
}

int main(int argc, char* argv[]) {
    float* buffer = new float[1024*1024];

    auto start = hrclock::now(), end = hrclock::now();
    std::vector<float> times(1000);

    for(int i = 0; i != 1000; ++i) {
        start = hrclock::now();
        render_noiseC(1024, 1024, buffer);
        end = hrclock::now();
        times[i] = duration_t(end - start).count();
    }

    float min = std::numeric_limits<float>::max(), max = -min, avg = 0.f;
    for(auto& v : times) {
        if(v < min) min = v;
        if(v > max) max = v;
        avg += v;
    }

    std::cout << "Time measured for CPU, total: " << avg << ", avg: " << avg/iterator_count << ", min: " << min << ", max: " << max << std::endl;

    for(int i = 0; i != 10; ++i) {
        std::cout << buffer[i] << ", ";
    }
    std::cout << std::endl;

    times.clear(); times.resize(1000);
    for(int i = 0; i != 1000; ++i) {
        start = hrclock::now();
        render_noiseB(1024, 1024, buffer);
        end = hrclock::now();
        times[i] = duration_t(end - start).count();
    }

    min = std::numeric_limits<float>::max(); max = -min; avg = 0.f;
    for(auto& v : times) {
        if(v < min) min = v;
        if(v > max) max = v;
        avg += v;
    }

    std::cout << "Time measured for SIMD, total: " << avg << ", avg: " << avg/iterator_count << ", min: " << min << ", max: " << max << std::endl;

    for(int i = 0; i != 10; ++i) {
        std::cout << buffer[i] << ", ";
    }
    std::cout << std::endl;

    delete[] buffer;
    return 0;
}

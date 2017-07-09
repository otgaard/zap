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

using namespace zap::maths::simd;

using hrclock = std::chrono::high_resolution_clock;
using duration_t = std::chrono::duration<float>;

// Compiled to leave assembly memory_access_tests.s

static const int width = 1024;
static const int height = 1024;
static const int px_count = width * height;
static const int iterator_count = 10000;

// SIMD constants

const veci32i seq_vi = { 3, 2, 1, 0 };
const vecm32f seq_vf = { 0.f, 1.f, 2.f, 3.f };
const veci32i zero_vi = _mm_setzero_si128();
const veci32i one_vi = { 1, 1, 1, 1 };
const vecm32f one_vf = { 1.f, 1.f, 1.f, 1.f };

float total(float* buffer_ptr, size_t count) {
    float result = 0.f;
    for(int i = 0; i != count; ++i)
        result += buffer_ptr[i];
    return result;
}

// Testing image generation

float lerp(float value, float A, float B) {
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
            buffer_ptr[offset + c] = lerp(c*inv_w, A, B);
        }
    }
}

inline vecm32f lerp(const vecm32f& value, const vecm32f& A, const vecm32f& B) {
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
            _mm_store_ps(buffer_ptr+offset+c, lerp(_mm_add_ps(load(c * inv_w), steps), A, B));
        }
    }
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

int main(int argc, char* argv[]) {
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
    return 0;
}

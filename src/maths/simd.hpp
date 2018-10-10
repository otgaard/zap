//
// Created by Darren Otgaar on 2017/07/07.
//

#ifndef ZAP_SIMD_HPP
#define ZAP_SIMD_HPP

#include <xmmintrin.h>  // SSE
#include <emmintrin.h>  // SSE2
#include <pmmintrin.h>  // SSE3
#include <tmmintrin.h>  // SSSE3
#include <smmintrin.h>  // SSE4.1
#include <initializer_list>
#include <cstdint>

#if defined(_WIN32)
#define VCALL _vectorcall
#elif defined(__APPLE__)
#define VCALL __attribute__((vectorcall))
#else
#define VCALL
#endif

#if defined(_WIN32)
#define VALIGN __declspec(align(16))
#else
#define VALIGN __attribute__((aligned(16)))
#endif

namespace zap { namespace maths { namespace simd {
    using vecm = __m128;
    using veci = __m128i;

    inline vecm VCALL load(const float* arr) { return _mm_load_ps(arr); }
    inline vecm VCALL load(float value) { return _mm_set1_ps(value); }
    inline veci VCALL load(int value) { return _mm_set1_epi32(value); }

#if defined(_WIN32)
    __declspec(align(16)) struct vecm32f {
#else
        struct __attribute__((aligned(16))) vecm32f {
#endif
        union {
            vecm v;
            float arr[4];
        };

        vecm32f() = default;
        explicit vecm32f(const float* arr_ptr) { arr[0] = *arr_ptr++, arr[1] = *arr_ptr++, arr[2] = *arr_ptr++, arr[3] = *arr_ptr; }
        vecm32f(__m128 vec) : v(vec) { }
        vecm32f(std::initializer_list<float> init) { auto it = init.begin(); arr[0] = *it++; arr[1] = *it++; arr[2] = *it++, arr[3] = *it; }
        operator __m128() const { return v; }
        operator float* const() const { return (float* const)arr; }
    };

#if defined(WIN32)
    __declspec(align(16)) struct vecm32i {
#else
        struct __attribute__((aligned(16))) vecm32i {
#endif
        union {
            vecm v;
            int32_t arr[4];
        };

        vecm32i() = default;
        explicit vecm32i(const int32_t* arr_ptr) { arr[0] = *arr_ptr++, arr[1] = *arr_ptr++, arr[2] = *arr_ptr++, arr[3] = *arr_ptr; }
        vecm32i(__m128 vec) : v(vec) { }
        vecm32i(std::initializer_list<int32_t> init) { auto it = init.begin(); arr[0] = *it++; arr[1] = *it++; arr[2] = *it++, arr[3] = *it; }
        operator __m128() const { return v; }
        operator int32_t* const() const { return (int32_t* const)arr; }
    };

#if defined(WIN32)
    __declspec(align(16)) struct veci32i {
#else
        struct __attribute__((aligned(16))) veci32i {
#endif
        union {
            veci v;
            int32_t arr[4];
        };

        veci32i() = default;
        explicit veci32i(const int32_t* arr_ptr) { arr[0] = *arr_ptr++, arr[1] = *arr_ptr++, arr[2] = *arr_ptr++, arr[3] = *arr_ptr; }
        explicit veci32i(const veci& vec) : v(vec) { }
        veci32i(__m128i vec) : v(vec) { }
        veci32i(std::initializer_list<int32_t> init) { auto it = init.begin(); arr[0] = *it++; arr[1] = *it++; arr[2] = *it++, arr[3] = *it; }
        operator __m128i() const { return v; }
        operator int32_t* const() const { return (int32_t* const)arr; }
    };

    //inline vecm VCALL load(int value) { vecm32i r = {{ (float)value, (float)value, (float)value, (float)value}}; return r.v; }

    const float mask = (float)255;
    const vecm32i vecm_abs_mask = { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF };
    const vecm32i vecm_rnd_mask = { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF };
    const veci32i veci_rnd_mask = { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF };
    const vecm32f vecm_no_fraction = { 8388608.f, 8388608.f, 8388608.f, 8388608.f };
    const vecm32f vecm_one = { 1.f, 1.f, 1.f, 1.f };
    const veci veci_one = _mm_cvtps_epi32(vecm_one.v);

    inline void set_round_down() {
        _MM_SET_ROUNDING_MODE(_MM_ROUND_DOWN);
    }

    inline void set_round_default() {
        _MM_SET_ROUNDING_MODE(_MM_ROUND_NEAREST);
    }

    inline vecm VCALL floor_v(const vecm& v) {
        veci test = _mm_and_si128(_mm_castps_si128(v), _mm_castps_si128(vecm_abs_mask.v));
        test = _mm_cmplt_epi32(test, _mm_castps_si128(vecm_no_fraction.v));
        veci vint = _mm_cvttps_epi32(v);
        vecm result = _mm_cvtepi32_ps(vint);
        vecm larger = _mm_cmpgt_ps(result, v);
        larger = _mm_cvtepi32_ps(_mm_castps_si128(larger));
        result = _mm_add_ps(result, larger);
        result = _mm_and_ps(result, _mm_castsi128_ps(test));
        test = _mm_andnot_si128(test, _mm_castps_si128(v));
        result = _mm_or_ps(result, _mm_castsi128_ps(test));
        return result;
    }

    inline vecm VCALL ffloor_v(const vecm& v) {
        vecm f = _mm_cvtepi32_ps(_mm_cvttps_epi32(v));
        return _mm_sub_ps(f, _mm_and_ps(_mm_cmplt_ps(v, f), vecm_one.v));
    }

    inline veci VCALL convert_v(const vecm& v) {
        return _mm_cvtps_epi32(v);
    }

    inline vecm VCALL lerp_v(const vecm& v0, const vecm& v1, const vecm& u) {
        return _mm_add_ps(
                _mm_mul_ps(_mm_sub_ps(vecm_one, u), v0),
                _mm_mul_ps(u, v1));
    }

    inline vecm VCALL bilinear_v(const vecm& P00, const vecm& P01, const vecm& P10, const vecm& P11, const vecm& u, const vecm& v) {
        return lerp_v(lerp_v(P00, P01, u), lerp_v(P10, P11, u), v);
    }

    // From Intel Developer Zone (SSE2 signed 32bit integer multiplication)
    // https://software.intel.com/en-us/forums/intel-c-compiler/topic/288768
    // _mm_mul_epu32  - Multiply the low unsigned 32-bit integers from each packed 64-bit element in a and b, and store
    //                  the unsigned 64-bit result in dst.
    // _mm_srli_si128 - shift a right by imm8 bytes while shifting in zeros and store the results in dst;
    // _mm_shuffle_ps - select first two dwords from a and last two dwords from b as per the dword arrangement in _MM_SHUFFLE()
    inline veci VCALL mul(const veci& a, const veci& b) {
        vecm tmp1 = _mm_castsi128_ps(_mm_mul_epu32(a,b));
        vecm tmp2 = _mm_castsi128_ps(_mm_mul_epu32(_mm_srli_si128(a,4), _mm_srli_si128(b,4)));
        return _mm_shuffle_epi32(_mm_castps_si128(_mm_shuffle_ps(tmp1, tmp2, _MM_SHUFFLE(2,0,2,0))), _MM_SHUFFLE(3,1,2,0));
    }

    inline vecm set_x(const vecm& v, float x) {
        vecm tmp = _mm_unpacklo_ps(load(x), v);
        return _mm_shuffle_ps(tmp, v, _MM_SHUFFLE(3, 2, 3, 0));
    }

    inline vecm set_y(const vecm& v, float y) {
        vecm tmp = _mm_unpacklo_ps(load(y), v);
        return _mm_shuffle_ps(tmp, v, _MM_SHUFFLE(3, 2, 0, 1));
    }

    inline vecm set_z(const vecm& v, float z) {
        vecm tmp = _mm_unpackhi_ps(load(z), v);
        return _mm_shuffle_ps(v, tmp, _MM_SHUFFLE(3, 0, 1, 0));
    }

    inline vecm set_w(const vecm& v, float w) {
        vecm tmp = _mm_unpackhi_ps(load(w), v);
        return _mm_shuffle_ps(v, tmp, _MM_SHUFFLE(0, 1, 1, 0));
    }

    inline vecm set(const vecm& v, int i, float value) {
        switch(i) {
            case 0: return set_x(v, value);
            case 1: return set_y(v, value);
            case 2: return set_z(v, value);
            case 3: return set_w(v, value);
            default: return v;
        }
    }

}}}

#endif //ZAP_SIMD_HPP

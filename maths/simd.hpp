//
// Created by Darren Otgaar on 2017/07/07.
//

#ifndef ZAP_SIMD_HPP
#define ZAP_SIMD_HPP

#include <xmmintrin.h>  // SSE
#include <emmintrin.h>  // SSE2
#include <pmmintrin.h>  // SSE3

#if defined(_WIN32)
#define VCALL _vectorcall
#else
#define VCALL __attribute__((vectorcall))
#endif

namespace zap { namespace maths { namespace simd {
    using vecm = __m128;
    using veci = __m128i;

    inline vecm VCALL load(const float* arr) { return _mm_load_ps(arr); }
    inline vecm VCALL load(float value) { return _mm_load_ps1(&value); }

#if defined(_WIN32)
    __declspec(align(16)) struct vecm32f {
#else
    struct __attribute__((aligned(16))) vecm32f {
#endif
        union {
            vecm v;
            float arr[4];
        };
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
    };

    const vecm32i vecm_abs_mask = {{ 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF }};
    const vecm32f vecm_no_fraction = {{ 8388608.f, 8388608.f, 8388608.f, 8388608.f }};

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

    inline veci VCALL floor_vi(const vecm& v) {
        return _mm_cvtps_epi32(v);
    }

    inline vecm VCALL lerp_v(const vecm& u, const vecm& v0, const vecm& v1) {
        return _mm_add_ps(
                _mm_mul_ps(_mm_sub_ps(load(1.f), u), v0),
                _mm_mul_ps(u, v1));
    }

    inline vecm VCALL bilinear_v(const vecm& u, const vecm& v, const vecm& P00, const vecm& P01, const vecm& P10, const vecm& P11) {
        return lerp_v(v, lerp_v(u, P00, P01), lerp_v(u, P10, P11));
    }

}}}

#endif //ZAP_SIMD_HPP

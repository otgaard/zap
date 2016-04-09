/* Created by Darren Otgaar on 2016/03/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_MATHS_HPP
#define ZAP_MATHS_HPP

#include <core/core.hpp>
#include <cmath>
#include <array>
#include <limits>
#include <cassert>
#include <numeric>
#include <type_traits>

namespace zap { namespace maths {
    template <typename T> inline bool eq(const T& a, const T& b) { return std::abs(a - b) < std::numeric_limits<T>::epsilon(); }
    template <typename T> inline bool eq(const T& a, const T& b, const T& epsilon) { return std::abs(a - b) < epsilon; }
    template <typename T> inline bool leq(const T& a, const T& b) { return a < b + std::numeric_limits<T>::epsilon(); }
    template <typename T> inline bool is_zero(const T& a) { return std::abs(T(0) - a) < std::numeric_limits<T>::epsilon(); }
    template <typename T> constexpr T sign(const T& s) { return (s > T(0)) ? T(1) : s < T(0) ? T(-1) : T(0); }
    template <typename T> constexpr T abs(const T& s) { return sign(s)*s; }
    template <typename T> inline T sgn(const T& s) { return is_zero(s) ? T(0) : sign(s); }
    template <typename T> constexpr T clamp(const T& v, const T& min, const T& max) { return (v < min) ? min : (v > max) ? max : v; }
    template <typename T> constexpr T clamp(const T& v) { return clamp(v, T(0), T(1)); }
    template <typename T> constexpr T min(const T& a, const T& b) { return (a < b) ? a : b; }
    template <typename T> constexpr T max(const T& a, const T& b) { return (a > b) ? a : b; }

    template <typename T> T constexpr log2_pow2(T po2) {
        return ((po2 & 0xAAAAAAAA) != 0) |
               ((po2 & 0xFFFF0000) != 0) << 4 |
               ((po2 & 0xFF00FF00) != 0) << 3 |
               ((po2 & 0xF0F0F0F0) != 0) << 2 |
               ((po2 & 0xCCCCCCCC) != 0) << 1;
    }

    template <size_t k> struct pow2;
    template <> struct pow2<0> { constexpr static size_t value = 1; };
    template <size_t k> struct pow2 { constexpr static size_t value = 2 * pow2<k-1>::value; };

    // Type and size safe static initialiser
    template <typename T, size_t N>
    struct static_list : std::array<T, N> {
        template <typename... Args>
        constexpr static_list(Args... args) : std::array<T, N>( {{ args... }} ) {
            static_assert(sizeof...(args) == N, ZERR_ARG_COUNT_INVALID);
        }
    };

    template <typename T>
    constexpr T csqrt_impl(T value, T curr, T prev) {
        static_assert(std::is_floating_point<T>::value != 0, ZERR_TYPE_INVALID);
        return (curr == prev) ? curr : csqrt_impl(value, T(0.5) * (curr + value / curr), curr);
    }

    template <typename T>
    constexpr T csqrt(T value) {
        return (value >= 0 && value < std::numeric_limits<T>::infinity())
               ? csqrt_impl(value, value, T(0))
               : std::numeric_limits<T>::quiet_NaN();
    }

    // Forward Declarations
    template <typename T> struct vec2;
    template <typename T> struct vec3;
    template <typename T> struct vec4;
}}

#endif //ZAP_MATHS_HPP

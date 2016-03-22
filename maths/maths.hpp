/* Created by Darren Otgaar on 2016/03/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_MATHS_HPP
#define ZAP_MATHS_HPP

#include <cmath>
#include <array>
#include <cassert>
#include <numeric>
#include <type_traits>

#define ZERR_IDX_OUT_OF_RANGE "Index out of range"
#define ZERR_ARG_COUNT_INVALID "Incorrect number of values provided"
#define ZERR_TYPE_INVALID "Invalid type used for template instantiation"
#define ZERR_TYPE_FLOATING "Requires floating-point type"

namespace zap { namespace maths {
    template <typename T> inline bool eq(T a, T b) { return std::abs(a - b) < std::numeric_limits<T>::epsilon(); }
    template <typename T> inline bool eq(T a, T b, T epsilon) { return std::abs(a - b) < epsilon; }
    template <typename T> inline bool leq(T a, T b) { return a < b + std::numeric_limits<T>::epsilon(); }
    template <typename T> inline bool is_zero(T a) { return std::abs(T(0) - a) < std::numeric_limits<T>::epsilon(); }
    template <typename T> constexpr T sign(T s) { return s > T(0) ? T(1) : s < T(0) ? T(-1) : T(0); }
    template <typename T> constexpr T abs(T s) { return sign(s)*s; }
    template <typename T> inline T sgn(T s) { return is_zero(s) ? T(0) : sign(s); }
    template <typename T> constexpr static T clamp(T v, T min, T max) { return v < min ? min : v > max ? max : v; }
    template <typename T> constexpr static T clamp(T v) { return clamp(v, T(0), T(1)); }

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
        return curr == prev ? curr : csqrt_impl(value, T(0.5) * (curr + value / curr), curr);
    }

    template <typename T>
    constexpr T csqrt(T value) {
        return value >= 0 && value < std::numeric_limits<T>::infinity()
               ? csqrt_impl(value, value, T(0))
               : std::numeric_limits<T>::quiet_NaN();
    }

    // Forward Declarations
    template <typename T> class vec2;
    template <typename T> class vec3;
    template <typename T> class vec4;
}}

#endif //ZAP_MATHS_HPP

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

#if defined(_WIN32)
#if !defined(MATHS_EXPORT)
#if defined(ZAP_STATIC)
#include "maths_exports_s.h"
#else
#include "maths_exports.h"
#endif
#else
#include MATHS_EXPORT
#endif
#define ALIGN_DECL(w) __declspec(align(w))
#define ALIGN_ATTR(w)
#else //_WIN32
#define ZAPMATHS_EXPORT
#define ALIGN_DECL(w)
#define ALIGN_ATTR(w) __attribute__((aligned(w)))
#endif //_WIN32

namespace zap { namespace maths {
#if defined(_WIN32)
    template <typename T> const T PI = T(3.14159265358979323846);
    template <typename T> const T TWO_PI = T(2) * PI<T>;
    template <typename T> const T HALF_PI = PI<T>/T(2);
    template <typename T> const T degrees_to_radians = PI<T>/T(180);
    template <typename T> const T radians_to_degrees = T(180)/PI<T>;

    template <typename T> const T deg_to_rad(T v) { return T(v * degrees_to_radians<T>); }
    template <typename T> const T rad_to_deg(T v) { return T(v * radians_to_degrees<T>); }
#else
    template <typename T> constexpr T PI = T(3.14159265358979323846);
    template <typename T> constexpr T TWO_PI = T(2) * PI<T>;
    template <typename T> constexpr T HALF_PI = PI<T>/T(2);
    template <typename T> constexpr T degrees_to_radians = PI<T>/T(180);
    template <typename T> constexpr T radians_to_degrees = T(180)/PI<T>;

    template <typename T> constexpr T deg_to_rad(T v) { return T(v * degrees_to_radians<T>); }
    template <typename T> constexpr T rad_to_deg(T v) { return T(v * radians_to_degrees<T>); }
#endif

    template <typename T> inline bool eq(const T& a, const T& b) { return std::abs(a - b) < std::numeric_limits<T>::epsilon(); }
    template <typename T> inline bool eq(const T& a, const T& b, const T& epsilon) { return std::abs(a - b) < epsilon; }
    template <typename T> inline bool leq(const T& a, const T& b) { return a < b + std::numeric_limits<T>::epsilon(); }
    template <typename T> constexpr T sign(const T& s) { return (s > T(0)) ? T(1) : s < T(0) ? T(-1) : T(0); }
    template <typename T> constexpr T abs(const T& s) { return sign(s)*s; }
    template <typename T> inline bool is_zero(const T& a) { return abs(T(0) - a) < std::numeric_limits<T>::epsilon(); }
    template <typename T> inline bool is_zero(const T& a, const T& epsilon) { return abs(T(0) - a) < epsilon; }
    template <typename T> inline T sgn(const T& s) { return is_zero(s) ? T(0) : sign(s); }
    template <typename T> constexpr T clamp(const T& v, const T& min, const T& max) { return (v < min) ? min : (v > max) ? max : v; }
    template <typename T> constexpr T clamp(const T& v) { return clamp(v, T(0), T(1)); }
    template <typename T> constexpr T min(const T& a, const T& b) { return (a < b) ? a : b; }
    template <typename T> constexpr T max(const T& a, const T& b) { return (a > b) ? a : b; }

    template <typename T> constexpr int floor(T x) { return ((int)x - (x < 0 && x != (int)x)); }

    // Forward Declarations
    template <typename T> struct vec2;
    template <typename T> struct vec3;
    template <typename T> struct vec4;
    template <typename T> struct mat2;
    template <typename T> struct mat3;
    template <typename T> struct mat4;

    // Clamp specialisations
    template <typename T>
    constexpr vec2<T> clamp(const vec2<T>& v, const vec2<T>& min, const vec2<T>& max) {
        return vec2<T>(clamp<T>(v.x, min.x, max.x), clamp<T>(v.y, min.y, max.y));
    }

    template <typename T>
    constexpr vec3<T> clamp(const vec3<T>& v, const vec3<T>& min, const vec3<T>& max) {
        return vec3<T>(clamp<T>(v.x, min.x, max.x), clamp<T>(v.y, min.y, max.y), clamp<T>(v.z, min.z, max.z));
    }

    template <typename T>
    constexpr vec4<T> clamp(const vec4<T>& v, const vec4<T>& min, const vec4<T>& max) {
        return vec4<T>(clamp<T>(v.x, min.x, max.x), clamp<T>(v.y, min.y, max.y), clamp<T>(v.z, min.z, max.z),
                       clamp<T>(v.w, min.w, max.w));
    }

    template <typename T, typename S> T lerp(const S& u, const T& P0, const T& P1) {
        return T((S(1) - u)*P0 + u*P1);
    };

    inline byte lerp(byte u, byte P0, byte P1) {
        return byte(((P0 * (255U - u)) >> 8) + ((u * P1) >> 8));
    }

    template <typename T, typename S>
    T bilinear(const S& u, const S& v, const T& P00, const T& P01, const T& P10, const T& P11) {
        //const auto omu = S(1) - u; return (S(1)-v)*(P00*omu + P01*u) + v*(P10*omu + P11*u);
        return lerp(v, lerp(u, P00, P01), lerp(u, P10, P11));
    }

    template <typename T, typename S>
    T trilinear(const S& u, const S& v, const S& w,
                const T& P000, const T& P010, const T& P100, const T& P110,
                const T& P001, const T& P011, const T& P101, const T& P111) {
        return lerp(w, lerp(v, lerp(u, P000, P010), lerp(u, P100, P110)),
                       lerp(v, lerp(u, P001, P011), lerp(u, P101, P111)));
    }

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

    template <typename T>
    constexpr T scale_bias(const T& value, const T& scale, const T& bias) {
        return scale*value + bias;
    }

    template <typename T>
    T wrap(const T& value, const T& min, const T& max) {
        return value < min ? max + std::fmod(value, min) : value > max ? min + std::fmod(value, max) : value;
    }

    template <typename T>
    struct loop {
        inline loop(T min, T max, T value=T(0), T dir=T(1)) : min(min), max(max), value(value), dir(dir) { }

        float operator+=(const T& t) {
            value += dir * t;
            if(value < min || value > max) {
                dir *= T(-1);
                value = clamp(value, min, max);
            }
            return value;
        }
        T min, max, value, dir;
    };

    template <typename Container, typename Fnc, typename T = typename std::result_of<Fnc&(typename Container::value_type)>::type>
    std::tuple<T, T> find_range(const Container& container, Fnc&& op) {
        T min = std::numeric_limits<T>::max(), max = std::numeric_limits<T>::lowest();
        for(const auto& el : container) {
            const auto val = op(el);
            if(val < min) min = val;
            if(val > max) max = val;
        }
        return std::make_tuple(min, max);
    }

    template <typename Container, typename T=typename Container::value_type>
    size_t find_interval_idx(const Container& container, T value, bool loop=false) {
        if(loop) {
            for(size_t i = 0, end = container.size()-1; i != end; ++i) {
                if(container[i] <= value && value < container[i+1]) return i;
            }
            return container.size()-1;
        } else {
            for(size_t i = 0, end = container.size()-1; i != end; ++i) {
                if(container[i] <= value && value < container[i+1]) return i;
            }
            return container.size()-1;
        }
    }
}}

#endif //ZAP_MATHS_HPP

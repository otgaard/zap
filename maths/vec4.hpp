//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_VEC4_HPP
#define ZAP_VEC4_HPP

#include "maths.hpp"

#ifdef ZAP_MATHS_SSE2
#include <xmmintrin.h>
#endif

namespace zap { namespace maths {
    template <typename T>
    struct vec4 {
    public:
        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "vec4<T>: T must be an algebraic pod_t");
        constexpr size_t size() const { return 4; }
        constexpr size_t memsize() const { return sizeof(vec4); }

        constexpr vec4() { };
        constexpr vec4(const static_list<T, 4>& lst) : x(lst[0]), y(lst[1]), z(lst[2]), w(lst[3]) { }
        constexpr vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }
        constexpr vec4(const vec2<T>& rhs, T z, T w) : x(rhs.x), y(rhs.y), z(z), w(w) { }
        constexpr vec4(const vec3<T>& rhs, T w) : x(rhs.x), y(rhs.y), z(rhs.z), w(w) { }
        constexpr vec4(const vec4<T>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) { }

        inline vec4<T>& operator=(const vec4<T>& rhs) {
            if(this != &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; }
            return *this;
        }

        inline T* begin() { return arr; }
        inline T* end() { return arr + size(); }
        inline const T* begin() const { return arr; }
        inline const T* end() const { return arr + size(); }
        inline const T* data() const { return arr; }
        inline T* data() { return arr; }

        inline T& operator[](size_t idx) {
            assert(idx < size() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx];
        }

        inline const T& operator[](size_t idx) const {
            assert(idx < size() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx];
        }

        union {
            struct {
                T x, y, z, w;
            };
            T arr[4];
#ifdef ZAP_MATHS_SSE2
            __m128 xmm;
#endif
        };
    } __attribute__((aligned(16)));

    using vec4b = vec4<uint8_t>;
    using vec4i = vec4<int>;
    using vec4f = vec4<float>;
    using vec4d = vec4<double>;

}}

#endif //ZAP_VEC4_HPP

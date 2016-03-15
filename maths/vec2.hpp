//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_VEC2_HPP
#define ZAP_VEC2_HPP

#include "maths.hpp"

namespace zap { namespace maths {
    template <typename T>
    class vec2 {
    public:
        constexpr static size_t size() { return 2; }
        constexpr static size_t memsize() { return sizeof(vec2); }

        constexpr vec2() { }
        constexpr vec2(const static_list<T, 2>& lst) : x(lst[0]), y(lst[1]) { }
        constexpr vec2(T x, T y) : x(x), y(y) { }
        constexpr vec2(const vec2<T>& rhs) : x(rhs.x), y(rhs.y) { }
        constexpr vec2(const vec3<T>& rhs) : x(rhs.x), y(rhs.y) { }
        constexpr vec2(const vec4<T>& rhs) : x(rhs.x), y(rhs.y) { }

        inline vec2<T>& operator=(const vec2<T>& rhs) {
            if(this != &rhs) { x = rhs.x; y = rhs.y; }
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

        union {
            struct {
                T x, y;
            };
            T arr[2];
        };
    } __attribute__((aligned(8)));

    template <typename T>
    constexpr vec2<T> operator+(const vec2<T>& lhs, const vec2<T>& rhs) {
        return vec2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    template <typename T>
    constexpr vec2<T>operator-(const vec2<T>& lhs, const vec2<T>& rhs) {
        return vec2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
    }
}}

#endif //ZAP_VEC2_HPP

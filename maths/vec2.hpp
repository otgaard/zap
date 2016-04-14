//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_VEC2_HPP
#define ZAP_VEC2_HPP

#include "maths.hpp"

namespace zap { namespace maths {
    template <typename T>
    struct vec2 {
    public:
        using type = T;
        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "vec2<T>: T must be an algebraic type");
        constexpr static size_t size() { return 2; }
        constexpr static size_t memsize() { return sizeof(vec2<T>); }

        constexpr vec2() { }
        constexpr explicit vec2(T init) : x(init), y(init) { }
        constexpr explicit vec2(const T* ptr) : x(ptr[0]), y(ptr[1]) { }
        constexpr vec2(T x, T y) : x(x), y(y) { }
        constexpr vec2(const static_list<T, 2>& lst) : x(lst[0]), y(lst[1]) { }
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

        inline T& operator[](size_t idx) { assert(idx < size() && ZERR_IDX_OUT_OF_RANGE); return arr[idx]; }
        inline const T& operator[](size_t idx) const { assert(idx < size() && ZERR_IDX_OUT_OF_RANGE); return arr[idx]; }

        constexpr vec2<T> operator-() const { return vec2<T>(-x, -y); }
        inline vec2<T>& operator+=(const vec2<T>& rhs) { for(size_t i = 0; i != size(); ++i) arr[i] += rhs.arr[i]; }

        union {
            struct {
                T x, y;
            };
            T arr[2];
        };
	}; // __attribute__((aligned(8)));

    template <typename T>
    constexpr vec2<T> operator+(const vec2<T>& lhs, const vec2<T>& rhs) {
        return vec2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    template <typename T>
    constexpr vec2<T>operator-(const vec2<T>& lhs, const vec2<T>& rhs) {
        return vec2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    using vec2b = vec2<uint8_t>;
    using vec2i = vec2<int>;
    using vec2f = vec2<float>;
    using vec2d = vec2<double>;

}}

#endif //ZAP_VEC2_HPP

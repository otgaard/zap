/* Created by Darren Otgaar on 2016/03/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_VEC3_HPP
#define ZAP_VEC3_HPP

#include "maths.hpp"

#ifdef ZAP_MATHS_SSE2
#include <xmmintrin.h>
#endif

template <typename T>
class vec3 {
public:
    constexpr inline size_t size() const { return 3; }
    constexpr inline size_t memsize() const { return sizeof(vec3); }

    constexpr inline vec3() { };
    constexpr inline vec3(const static_list<T, 3>& lst) : x(lst[0]), y(lst[1]), z(lst[2]) { }
    constexpr inline vec3(T x, T y, T z) : x(x), y(y), z(z) { }
    constexpr inline vec3(const vec3<T>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) { }

    inline vec3<T>& operator=(const vec3<T>& rhs) {
        if(this != &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; }
        return *this;
    }

    inline T* begin() { return arr; }
    inline T* end() { return arr+size(); }
    inline const T* begin() const { return arr; }
    inline const T* end() const { return arr+size(); }

    inline const T* data() const { return arr; }
    inline T* data() { return arr; }

    inline T& operator[](size_t idx) { assert(idx < size() && ZERR_IDX_OUT_OF_RANGE); return arr[idx]; }

    union {
        struct {
            T x, y, z;
        };
        T arr[4];
#ifdef ZAP_MATHS_SSE2
        __m128 xmm;
#endif
    };
} __attribute__((aligned(16)));

template <typename T>
constexpr inline vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) {
    return vec3<T>(lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z);
}

template <typename T>
constexpr inline vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) {
    return vec3<T>(lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z);
}

template <typename T>
constexpr inline vec3<T> operator*(const vec3<T>& lhs, const vec3<T>& rhs) {
    return vec3<T>(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z);
}

template <typename T>
constexpr inline vec3<T> operator/(const vec3<T>& lhs, const vec3<T>& rhs) {
    return vec3<T>(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z);
}

template <typename T>
constexpr inline vec3<T> operator*(T scalar, const vec3<T>& rhs) {
    return vec3<T>(scalar*rhs.x, scalar*rhs.y, scalar*rhs.z);
}

template <typename T>
constexpr inline vec3<T> operator*(const vec3<T>& rhs, T scalar) { return operator*(scalar, rhs); }

template <typename T>
constexpr inline vec3<T> operator/(const vec3<T>& lhs, T scalar) {
    const T inv_scalar = T(1)/scalar;
    return vec3<T>(lhs.x*inv_scalar, lhs.y*inv_scalar, lhs.z*inv_scalar);
}

template <typename T>
constexpr inline vec3<T> operator/(T scalar, const vec3<T>& rhs) {
    return vec3<T>(scalar/rhs.x, scalar/rhs.y, scalar/rhs.z);
}

template <typename T>
constexpr inline T dot(const vec3<T>& lhs, const vec3<T>& rhs) {
    return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}

#endif //ZAP_VEC3_HPP

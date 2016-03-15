/* Created by Darren Otgaar on 2016/03/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_VEC3_HPP
#define ZAP_VEC3_HPP

#include "maths.hpp"

#ifdef ZAP_MATHS_SSE2
#include <xmmintrin.h>
#endif

namespace zap { namespace maths {
    template <typename T> constexpr inline T dot(const vec3<T>& lhs, const vec3<T>& rhs);

    template <typename T>
    class vec3 {
    public:
        constexpr static inline size_t size() { return 3; }
        constexpr static inline size_t memsize() { return sizeof(vec3<T>); }

        constexpr inline vec3() { };
        constexpr inline vec3(const static_list<T, 3>& lst) : x(lst[0]), y(lst[1]), z(lst[2]) { }
        constexpr inline vec3(T x, T y, T z) : x(x), y(y), z(z) { }
        constexpr inline vec3(const vec2<T>& rhs, T z=T(0)) : x(rhs.x), y(rhs.y), z(z) { }
        constexpr inline vec3(const vec3<T>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) { }
        constexpr inline vec3(const vec4<T>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) { }

        inline vec3<T>& operator=(const vec3<T>& rhs) {
            if(this != &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; }
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

        constexpr inline vec3<T> operator-() const { return vec3<T>(-x, -y, -z); }
        inline vec3<T>& operator+=(const vec3<T>& rhs) { for(size_t i = 0; i != size(); ++i) arr[i] += rhs.arr[i]; }
        inline vec3<T>& operator-=(const vec3<T>& rhs) { for(size_t i = 0; i != size(); ++i) arr[i] -= rhs.arr[i]; }
        inline vec3<T>& operator*=(const vec3<T>& rhs) { for(size_t i = 0; i != size(); ++i) arr[i] *= rhs.arr[i]; }
        inline vec3<T>& operator*=(T scalar) { for(size_t i = 0; i != size(); ++i) arr[i] *= scalar; }
        inline vec3<T>& operator/=(const vec3<T>& rhs) { for(size_t i = 0; i != size(); ++i) arr[i] /= rhs.arr[i]; }
        inline vec3<T>& operator/=(T scalar) { for(size_t i = 0; i != size(); ++i) arr[i] /= scalar; }

        constexpr inline T length_sqr() const { return dot(*this, *this); }
        inline T length() const { return std::sqrt(length_sqr()); }
        inline vec3<T>& normalise() { auto l = T(1)/length(); for(auto& v : arr) v *= l; return *this; }

        inline bool operator==(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] != rhs.arr[i]) return false;
            return true;
        }

        inline bool operator!=(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] == rhs.arr[i]) return false;
            return true;
        }

        inline bool operator<(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] >= rhs.arr[i]) return false;
            return true;
        }

        inline bool operator<=(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] > rhs.arr[i]) return false;
            return true;
        }

        inline bool operator>(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] <= rhs.arr[i]) return false;
            return true;
        }

        inline bool operator>=(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] < rhs.arr[i]) return false;
            return true;
        }

        inline bool eq(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(!eq(arr[i], rhs.arr[i])) return false;
            return true;
        }

        inline bool eq(const vec3<T>& rhs, T epsilon) const {
            for(size_t i = 0; i != size(); ++i) if(!maths::eq(arr[i], rhs.arr[i], epsilon)) return false;
            return true;
        }

        inline bool is_zero() const {
            for(size_t i = 0; i != size(); ++i) if(!maths::is_zero(arr[i])) return false;
            return true;
        }

        inline bool neq(const vec3<T>& rhs) const { return !eq(rhs); }
        inline bool is_unit() const { return length_sqr() == T(1); }

        union {
            struct {
                T x, y, z;
            };
            T arr[size()];
#ifdef ZAP_MATHS_SSE2
            __m128 xmm;
#endif
        };
    } __attribute__((aligned(16)));

    template <typename T>
    constexpr inline vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) {
        return vec3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    template <typename T>
    constexpr inline vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) {
        return vec3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    template <typename T>
    constexpr inline vec3<T> operator*(const vec3<T>& lhs, const vec3<T>& rhs) {
        return vec3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }

    template <typename T>
    constexpr inline vec3<T> operator/(const vec3<T>& lhs, const vec3<T>& rhs) {
        return vec3<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
    }

    template <typename T>
    constexpr inline vec3<T> operator*(T scalar, const vec3<T>& rhs) {
        return vec3<T>(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z);
    }

    template <typename T>
    constexpr inline vec3<T> operator*(const vec3<T>& rhs, T scalar) { return operator*(scalar, rhs); }

    template <typename T>
    constexpr inline vec3<T> operator/(const vec3<T>& lhs, T scalar) {
        const T inv_scalar = T(1) / scalar;
        return vec3<T>(lhs.x * inv_scalar, lhs.y * inv_scalar, lhs.z * inv_scalar);
    }

    template <typename T>
    constexpr inline vec3<T> operator/(T scalar, const vec3<T>& rhs) {
        return vec3<T>(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
    }

    template <typename T>
    constexpr inline T dot(const vec3<T>& lhs, const vec3<T>& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    template <typename T>
    inline vec3<T> normalise(const vec3<T>& v) {
        return T(1)/v.length() * v;
    }

    template <typename T>
    constexpr inline vec3<T> inverse(const vec3<T>& v) {
        return T(1)/v;
    }

    using vec3b = vec3<uint8_t>;
    using vec3i = vec3<int>;
    using vec3f = vec3<float>;
    using vec3d = vec3<double>;
}}

#endif //ZAP_VEC3_HPP

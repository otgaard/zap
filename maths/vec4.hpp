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
    struct ALIGN_DECL(16) vec4 {
    public:
        using type = T;
        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "vec4<T>: T must be an algebraic type");
        constexpr static size_t size() { return 4; }
        constexpr static size_t memsize() { return sizeof(vec4); }

        vec4() { }
        constexpr explicit vec4(T init) : x(init), y(init), z(init), w(init) { }
        constexpr explicit vec4(const T* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3]) { }
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

        inline T& operator[](size_t idx) { checkidx(idx,size()); return arr[idx]; }
        inline const T& operator[](size_t idx) const { checkidx(idx,size()); return arr[idx]; }

        void set(T nx, T ny, T nz, T nw) { x = nx; y = ny; z = nz; w = nw; }

        constexpr vec4<T> operator-() const { return vec4<T>(-x, -y, -z, -w); }

        inline vec4<T>& operator+=(const vec4<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] += rhs.arr[i];
            return *this;
        }

        inline vec4<T>& operator-=(const vec4<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] -= rhs.arr[i];
            return *this;
        }

        inline vec4<T>& operator*=(const vec4<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] *= rhs.arr[i];
            return *this;
        }

        inline vec4<T>& operator*=(T scalar) {
            for(size_t i = 0; i != size(); ++i) arr[i] *= scalar;
            return *this;
        }

        inline vec4<T>& operator/=(const vec4<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] /= rhs.arr[i];
            return *this;
        }

        inline vec4<T>& operator/=(T scalar) {
            for(size_t i = 0; i != size(); ++i) arr[i] /= scalar;
            return *this;
        }

        constexpr T length_sqr() const { return dot(*this, *this); }
        inline T length() const { return std::sqrt(length_sqr()); }
        inline vec4<T>& normalise() { auto l = T(1)/length(); for(auto& v : arr) v *= l; return *this; }

        inline bool operator==(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] != rhs.arr[i]) return false;
            return true;
        }

        inline bool operator!=(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] == rhs.arr[i]) return false;
            return true;
        }

        inline bool operator<(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] >= rhs.arr[i]) return false;
            return true;
        }

        inline bool operator<=(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] > rhs.arr[i]) return false;
            return true;
        }

        inline bool operator>(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] <= rhs.arr[i]) return false;
            return true;
        }

        inline bool operator>=(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] < rhs.arr[i]) return false;
            return true;
        }

        inline bool eq(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(!eq(arr[i], rhs.arr[i])) return false;
            return true;
        }

        inline bool eq(const vec4<T>& rhs, T epsilon) const {
            for(size_t i = 0; i != size(); ++i) if(!maths::eq(arr[i], rhs.arr[i], epsilon)) return false;
            return true;
        }

        inline bool is_zero() const {
            for(size_t i = 0; i != size(); ++i) if(!maths::is_zero(arr[i])) return false;
            return true;
        }

        inline bool neq(const vec4<T>& rhs) const { return !eq(rhs); }
        inline bool is_unit() const { return length_sqr() == T(1); }

        union {
            struct {
                T x, y, z, w;
            };
            T arr[4];
#ifdef ZAP_MATHS_SSE2
            __m128 xmm;
#endif
        };
	} ALIGN_ATTR(16);

        template <typename T>
        constexpr vec4<T> operator*(const vec4<T>& lhs, const vec4<T>& rhs) {
            return vec4<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
        }

        template <typename T>
        constexpr vec4<T> operator/(const vec4<T>& lhs, const vec4<T>& rhs) {
            return vec4<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
        }

        template <typename T>
        constexpr vec4<T> operator*(T scalar, const vec4<T>& rhs) {
            return vec4<T>(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w);
        }

        template <typename T>
        constexpr vec4<T> operator*(const vec4<T>& rhs, T scalar) { return operator*(scalar, rhs); }

        template <typename T>
        constexpr vec4<T> operator/(const vec4<T>& lhs, T scalar) {
            const T inv_scalar = T(1) / scalar;
            return vec4<T>(lhs.x * inv_scalar, lhs.y * inv_scalar, lhs.z * inv_scalar, lhs.w * inv_scalar);
        }

        template <typename T>
        constexpr vec4<T> operator/(T scalar, const vec4<T>& rhs) {
            return vec4<T>(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z, scalar / rhs.w);
        }

        template <typename T>
        constexpr T dot(const vec4<T>& lhs, const vec4<T>& rhs) {
            return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
        }

        template <typename T>
        inline vec4<T> normalise(const vec4<T>& v) {
            return T(1)/v.length() * v;
        }

        template <typename T>
        constexpr vec4<T> inverse(const vec4<T>& v) {
            return T(1)/v;
        }


        using vec4b = vec4<uint8_t>;
    using vec4s = vec4<int16_t>;
    using vec4i = vec4<int32_t>;
    using vec4f = vec4<float>;
    using vec4d = vec4<double>;

}}

#endif //ZAP_VEC4_HPP

//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_VEC4_HPP
#define ZAP_VEC4_HPP

#include <maths/maths.hpp>

#ifdef ZAP_MATHS_SSE2
#include <xmmintrin.h>
#endif

namespace zap { namespace maths {
    template <typename T>
    struct vec4 {
    public:
        using type = T;
        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "vec4<T>: T must be an algebraic type");
        constexpr static size_t size() { return 4; }
        constexpr static size_t bytesize() { return sizeof(vec4); }

        vec4() = default;
        constexpr explicit vec4(T init) : x(init), y(init), z(init), w(init) { }
        constexpr explicit vec4(const T* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]), w(ptr[3]) { }
        constexpr vec4(const static_list<T, 4>& lst) : x(lst[0]), y(lst[1]), z(lst[2]), w(lst[3]) { }
        constexpr vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }
        constexpr vec4(const vec2<T>& rhs, T z, T w) : x(rhs.x), y(rhs.y), z(z), w(w) { }
        constexpr vec4(const vec3<T>& rhs, T w) : x(rhs.x), y(rhs.y), z(rhs.z), w(w) { }
        constexpr vec4(const vec4<T>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w) { }

        vec4<T>& operator=(const vec4<T>& rhs) {
            if(this != &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; }
            return *this;
        }

        T* begin() { return arr; }
        T* end() { return arr + size(); }
        const T* begin() const { return arr; }
        const T* end() const { return arr + size(); }
        const T* data() const { return arr; }
        T* data() { return arr; }

        T& operator[](size_t idx) { checkidx(idx,size()); return arr[idx]; }
        const T& operator[](size_t idx) const { checkidx(idx,size()); return arr[idx]; }

        void set(const vec3<T>& v, T nw) { x = v.x; y = v.y; z = v.z; w = nw; }
        void set(T nx, T ny, T nz, T nw) { x = nx; y = ny; z = nz; w = nw; }

        constexpr vec4<T> operator-() const { return vec4<T>(-x, -y, -z, -w); }

        vec4<T>& operator+=(const vec4<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] += rhs.arr[i];
            return *this;
        }

        vec4<T>& operator-=(const vec4<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] -= rhs.arr[i];
            return *this;
        }

        vec4<T>& operator*=(const vec4<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] *= rhs.arr[i];
            return *this;
        }

        vec4<T>& operator*=(T scalar) {
            for(size_t i = 0; i != size(); ++i) arr[i] *= scalar;
            return *this;
        }

        vec4<T>& operator/=(const vec4<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] /= rhs.arr[i];
            return *this;
        }

        vec4<T>& operator/=(T scalar) {
            for(size_t i = 0; i != size(); ++i) arr[i] /= scalar;
            return *this;
        }

        constexpr T length_sqr() const { return dot(*this, *this); }
        T length() const { return std::sqrt(length_sqr()); }
        vec4<T>& normalise() { auto l = T(1)/length(); for(auto& v : arr) v *= l; return *this; }

        bool operator==(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] != rhs.arr[i]) return false;
            return true;
        }

        bool operator!=(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] == rhs.arr[i]) return false;
            return true;
        }

        bool operator<(const vec4<T>& rhs) const {
            return !operator==(rhs);
        }

        bool operator<=(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] > rhs.arr[i]) return false;
            return true;
        }

        bool operator>(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] <= rhs.arr[i]) return false;
            return true;
        }

        bool operator>=(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] < rhs.arr[i]) return false;
            return true;
        }

        bool eq(const vec4<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(!eq(arr[i], rhs.arr[i])) return false;
            return true;
        }

        bool eq(const vec4<T>& rhs, T epsilon) const {
            for(size_t i = 0; i != size(); ++i) if(!maths::eq(arr[i], rhs.arr[i], epsilon)) return false;
            return true;
        }

        bool is_zero() const {
            for(size_t i = 0; i != size(); ++i) if(!maths::is_zero(arr[i])) return false;
            return true;
        }

        bool neq(const vec4<T>& rhs) const { return !eq(rhs); }
        bool is_unit() const { return eq(length_sqr(), T(1)); }

        vec3<T> xyz() const { return vec3<T>(x, y, z); }

        union {
            struct {
                T x, y, z, w;
            };
            struct {
                T r, g, b, a;
            };
            T arr[4];
#ifdef ZAP_MATHS_SSE2
            __m128 xmm;
#endif
        };
	} /*ALIGN_ATTR(16)*/;

    template <typename T>
    constexpr vec4<T> operator+(const vec4<T>& lhs, const vec4<T>& rhs) {
        return vec4<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
    }

    template <typename T>
    constexpr vec4<T> operator-(const vec4<T>& lhs, const vec4<T>& rhs) {
        return vec4<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
    }

    template <typename T>
    constexpr vec4<T> operator*(const vec4<T>& lhs, const vec4<T>& rhs) {
        return vec4<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
    }

    template <typename T>
    constexpr vec4<T> operator/(const vec4<T>& lhs, const vec4<T>& rhs) {
        return vec4<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
    }

    template <typename S, typename T>
    constexpr vec4<T> operator*(const S& scalar, const vec4<T>& rhs) {
        return vec4<T>(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w);
    };

    template <typename S, typename T>
    constexpr vec4<T> operator*(const vec4<T>& rhs, const S& scalar) {
        return operator*(scalar, rhs);
    };

    template <typename T>
    constexpr vec4<T> operator*(const T& scalar, const vec4<T>& rhs) {
        return vec4<T>(scalar * rhs.x, scalar * rhs.y, scalar * rhs.z, scalar * rhs.w);
    }

    template <typename T>
    constexpr vec4<T> operator*(const vec4<T>& rhs, const T& scalar) { return operator*(scalar, rhs); }

    template <typename T>
    constexpr vec4<T> operator/(const vec4<T>& lhs, const T& scalar) {
        return vec4<T>(lhs.x / scalar, lhs.y / scalar, lhs.z / scalar, lhs.w / scalar);
    }

    template <typename T>
    constexpr vec4<T> operator/(const T& scalar, const vec4<T>& rhs) {
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
    using vec4u = vec4<uint32_t>;
    using vec4f = vec4<float>;
    using vec4d = vec4<double>;

}}

#endif //ZAP_VEC4_HPP

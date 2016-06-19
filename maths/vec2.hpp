//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_VEC2_HPP
#define ZAP_VEC2_HPP

#include "maths.hpp"

namespace zap { namespace maths {
    template <typename T>
    struct ALIGN_DECL(8) vec2 {
    public:
        using type = T;
        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "vec2<T>: T must be an algebraic type");
        constexpr static size_t size() { return 2; }
        constexpr static size_t memsize() { return sizeof(vec2<T>); }

        vec2() { }
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

        void set(T nx, T ny) { x = nx; y = ny; }

        constexpr vec2<T> operator-() const { return vec2<T>(-x, -y); }
        inline vec2<T>& operator+=(const vec2<T>& rhs) { for(size_t i = 0; i != size(); ++i) arr[i] += rhs.arr[i]; }

        vec2<T>& perp() { std::swap(x,y); x *= -1; return *this; }

        inline vec2<T>& operator-=(const vec2<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] -= rhs.arr[i];
            return *this;
        }

        inline vec2<T>& operator*=(const vec2<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] *= rhs.arr[i];
            return *this;
        }

        inline vec2<T>& operator*=(T scalar) {
            for(size_t i = 0; i != size(); ++i) arr[i] *= scalar;
            return *this;
        }

        inline vec2<T>& operator/=(const vec2<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] /= rhs.arr[i];
            return *this;
        }

        inline vec2<T>& operator/=(T scalar) {
            for(size_t i = 0; i != size(); ++i) arr[i] /= scalar;
            return *this;
        }

        constexpr T length_sqr() const { return x*x + y*y; }
        T length() const { return std::sqrt(length_sqr()); }

        inline vec2<T>& normalise() { auto l = T(1)/length(); for(auto& v : arr) v *= l; return *this; }

        inline bool operator==(const vec2<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] != rhs.arr[i]) return false;
            return true;
        }

        inline bool operator!=(const vec2<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] == rhs.arr[i]) return false;
            return true;
        }

        inline bool operator<(const vec2<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] >= rhs.arr[i]) return false;
            return true;
        }

        inline bool operator<=(const vec2<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] > rhs.arr[i]) return false;
            return true;
        }

        inline bool operator>(const vec2<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] <= rhs.arr[i]) return false;
            return true;
        }

        inline bool operator>=(const vec2<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] < rhs.arr[i]) return false;
            return true;
        }

        inline bool eq(const vec2<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(!eq(arr[i], rhs.arr[i])) return false;
            return true;
        }

        inline bool eq(const vec2<T>& rhs, T epsilon) const {
            for(size_t i = 0; i != size(); ++i) if(!maths::eq(arr[i], rhs.arr[i], epsilon)) return false;
            return true;
        }

        inline bool is_zero() const {
            for(size_t i = 0; i != size(); ++i) if(!maths::is_zero(arr[i])) return false;
            return true;
        }

        inline bool neq(const vec2<T>& rhs) const { return !eq(rhs); }
        inline bool is_unit() const { return length_sqr() == T(1); }

        union {
            struct {
                T x, y;
            };
            T arr[2];
        };
	} ALIGN_ATTR(8);

    template <typename T>
    constexpr vec2<T> perp(const vec2<T>& v) {
        return vec2<T>(-v.y, v.x);
    }

    template <typename T>
    constexpr vec2<T> operator+(const vec2<T>& lhs, const vec2<T>& rhs) {
        return vec2<T>(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    template <typename T>
    constexpr vec2<T>operator-(const vec2<T>& lhs, const vec2<T>& rhs) {
        return vec2<T>(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    template <typename T>
    constexpr vec2<T> operator*(const vec2<T>& lhs, const vec2<T>& rhs) {
        return vec3<T>(lhs.x * rhs.x, lhs.y * rhs.y);
    }

    template <typename T>
    constexpr vec2<T> operator/(const vec2<T>& lhs, const vec2<T>& rhs) {
        return vec3<T>(lhs.x / rhs.x, lhs.y / rhs.y);
    }

    template <typename T>
    constexpr vec2<T> operator*(T scalar, const vec2<T>& rhs) {
        return vec2<T>(scalar * rhs.x, scalar * rhs.y);
    }

    template <typename T>
    constexpr vec2<T> operator*(const vec2<T>& rhs, T scalar) { return operator*(scalar, rhs); }

    template <typename T>
    constexpr vec2<T> operator/(const vec2<T>& lhs, T scalar) {
        const T inv_scalar = T(1) / scalar;
        return vec2<T>(lhs.x * inv_scalar, lhs.y * inv_scalar);
    }

    template <typename T>
    constexpr vec2<T> operator/(T scalar, const vec2<T>& rhs) {
        return vec2<T>(scalar / rhs.x, scalar / rhs.y);
    }

    template <typename T>
    constexpr T dot(const vec2<T>& lhs, const vec2<T>& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y;
    }

    template <typename T>
    inline vec2<T> normalise(const vec2<T>& v) {
        return T(1)/v.length() * v;
    }

    template <typename T>
    constexpr vec2<T> inverse(const vec2<T>& v) {
        return T(1)/v;
    }

    using vec2b = vec2<uint8_t>;
    using vec2s = vec2<int16_t>;
    using vec2i = vec2<int32_t>;
    using vec2f = vec2<float>;
    using vec2d = vec2<double>;

}}

#endif //ZAP_VEC2_HPP

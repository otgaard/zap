/* Created by Darren Otgaar on 2016/03/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_VEC3_HPP
#define ZAP_VEC3_HPP

#include <maths/maths.hpp>

namespace zap { namespace maths {
    template <typename T> constexpr T dot(const vec3<T>& lhs, const vec3<T>& rhs);

    template <typename T>
    struct vec3 {
        using type = T;
        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "vec3<T>: T must be an algebraic type");
        constexpr static size_t size() { return 3; }
        constexpr static size_t bytesize() { return sizeof(vec3<T>); }

        vec3() = default;
        constexpr explicit vec3(T init) : x(init), y(init), z(init) { }
        constexpr explicit vec3(const T* ptr) : x(ptr[0]), y(ptr[1]), z(ptr[2]) { }
        constexpr vec3(T x, T y, T z) : x(x), y(y), z(z) { }
        constexpr vec3(const static_list<T, 3>& lst) : x(lst[0]), y(lst[1]), z(lst[2]) { }
        constexpr vec3(const vec2<T>& rhs, T z=T(0)) : x(rhs.x), y(rhs.y), z(z) { }
        constexpr vec3(const vec3<T>& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) { }

        vec3<T>& operator=(const vec3<T>& rhs) {
            if(this != &rhs) { x = rhs.x; y = rhs.y; z = rhs.z; }
            return *this;
        }

        T* begin() { return arr; }
        T* end() { return arr + size(); }
        const T* begin() const { return arr; }
        const T* end() const { return arr + size(); }
        const T* data() const { return arr; }
        T* data() { return arr; }

        T& operator[](size_t idx) { assert(idx < size() && ZERR_IDX_OUT_OF_RANGE); return arr[idx]; }
        const T& operator[](size_t idx) const { assert(idx < size() && ZERR_IDX_OUT_OF_RANGE); return arr[idx]; }

        void set(T nx, T ny, T nz) { x = nx; y = ny; z = nz; }

        constexpr vec3<T> operator-() const { return vec3<T>(-x, -y, -z); }

        vec3<T>& operator+=(const vec3<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] += rhs.arr[i];
            return *this;
        }

        vec3<T>& operator-=(const vec3<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] -= rhs.arr[i];
            return *this;
        }

        vec3<T>& operator*=(const vec3<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] *= rhs.arr[i];
            return *this;
        }

        vec3<T>& operator*=(T scalar) {
            for(size_t i = 0; i != size(); ++i) arr[i] *= scalar;
            return *this;
        }

        vec3<T>& operator/=(const vec3<T>& rhs) {
            for(size_t i = 0; i != size(); ++i) arr[i] /= rhs.arr[i];
            return *this;
        }

        vec3<T>& operator/=(T scalar) {
            for(size_t i = 0; i != size(); ++i) arr[i] /= scalar;
            return *this;
        }

        constexpr T length_sqr() const { return dot(*this, *this); }
        T length() const { return std::sqrt(length_sqr()); }
        vec3<T>& normalise() { auto l = T(1)/length(); for(auto& v : arr) v *= l; return *this; }

        bool operator==(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] != rhs.arr[i]) return false;
            return true;
        }

        bool operator!=(const vec3<T>& rhs) const {
            return !operator==(rhs);
        }

        bool operator<(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] >= rhs.arr[i]) return false;
            return true;
        }

        bool operator<=(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] > rhs.arr[i]) return false;
            return true;
        }

        bool operator>(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] <= rhs.arr[i]) return false;
            return true;
        }

        bool operator>=(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(arr[i] < rhs.arr[i]) return false;
            return true;
        }

        bool eq(const vec3<T>& rhs) const {
            for(size_t i = 0; i != size(); ++i) if(!eq(arr[i], rhs.arr[i])) return false;
            return true;
        }

        bool eq(const vec3<T>& rhs, T epsilon) const {
            for(size_t i = 0; i != size(); ++i) if(!maths::eq(arr[i], rhs.arr[i], epsilon)) return false;
            return true;
        }

        bool is_zero() const {
            for(size_t i = 0; i != size(); ++i) if(!maths::is_zero(arr[i])) return false;
            return true;
        }

        bool neq(const vec3<T>& rhs) const { return !eq(rhs); }
        bool is_unit() const { return maths::eq(length_sqr(), T(1)); }

        vec2<T> xy() const { return vec2<T>(x, y); }

        union {
            struct {
                T x, y, z;
            };
            struct {
                T r, g, b;
            };
            T arr[size()];
#ifdef ZAP_MATHS_SSE2
            __m128 xmm;
#endif
        };
    } /*ALIGN_ATTR(16)*/;

    template <typename T>
    constexpr vec3<T> operator+(const vec3<T>& lhs, const vec3<T>& rhs) {
        return vec3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    template <typename T>
    constexpr vec3<T> operator-(const vec3<T>& lhs, const vec3<T>& rhs) {
        return vec3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    template <typename T>
    constexpr vec3<T> operator*(const vec3<T>& lhs, const vec3<T>& rhs) {
        return vec3<T>(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }

    template <typename T>
    constexpr vec3<T> operator/(const vec3<T>& lhs, const vec3<T>& rhs) {
        return vec3<T>(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
    }

    template <typename T, typename S>
    constexpr vec3<T> operator*(S scalar, const vec3<T>& rhs) {
        return vec3<T>(T(scalar * rhs.x), T(scalar * rhs.y), T(scalar * rhs.z));
    }

    template <typename T, typename S>
    constexpr vec3<T> operator*(const vec3<T>& rhs, const S& scalar) {
        return operator*(scalar, rhs);
    }

    template <typename T>
    constexpr vec3<T> operator/(const vec3<T>& lhs, const T& scalar) {
        return vec3<T>(lhs.x / scalar, lhs.y / scalar, lhs.z / scalar);
    }

    template <typename T>
    constexpr vec3<T> operator/(const T& scalar, const vec3<T>& rhs) {
        return vec3<T>(scalar / rhs.x, scalar / rhs.y, scalar / rhs.z);
    }

    template <typename T>
    constexpr T dot(const vec3<T>& lhs, const vec3<T>& rhs) {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    template <typename T>
    constexpr T dot(const vec3<T>& lhs, const T& x, const T& y, const T& z) {
        return lhs.x * x + lhs.y * y + lhs.z * z;
    }

    template <typename T>
    inline vec3<T> normalise(const vec3<T>& v) {
        return T(1)/v.length() * v;
    }

    template <typename T>
    constexpr vec3<T> inverse(const vec3<T>& v) {
        return T(1)/v;
    }

    template <typename T>
    vec3<T> reciprocal(const vec3<T>& v) {
        T s01 = v[0]*v[1];
        T s02 = v[0]*v[2];
        T s12 = v[1]*v[2];
        T inv_s012 = T(1)/(s01*v[2]);
        return vec3<T>(s12*inv_s012, s02*inv_s012, s01*inv_s012);
    }

    template <typename T>
    constexpr vec3<T> cross(const vec3<T>& u, const vec3<T>& v) {
        return vec3<T>(u[1]*v[2]-u[2]*v[1], u[2]*v[0]-u[0]*v[2], u[0]*v[1]-u[1]*v[0]);
    }

    template <typename T>
    bool eq(const vec3<T>& A, const vec3<T>& B, T epsilon=std::numeric_limits<T>::epsilon()) {
        return A.eq(B, epsilon);
    }

    template <typename T>
    bool neq(const vec3<T>& A, const vec3<T>& B, T epsilon=std::numeric_limits<T>::epsilon()) {
        return !eq(A, B, epsilon);
    }

    using vec3b = vec3<uint8_t>;
    using vec3s = vec3<int16_t>;
    using vec3i = vec3<int32_t>;
    using vec3u = vec3<uint32_t>;
    using vec3f = vec3<float>;
    using vec3d = vec3<double>;

    inline vec3b lerp(byte u, const vec3b& P0, const vec3b& P1) {
        return vec3b {
                lerp(u, P0.r, P1.r),
                lerp(u, P0.g, P1.g),
                lerp(u, P0.b, P1.b) };
    }

}}

#endif //ZAP_VEC3_HPP

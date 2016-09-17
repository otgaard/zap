//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_MAT2_HPP
#define ZAP_MAT2_HPP

#include "maths.hpp"
#include "vec2.hpp"

/* Note:
 * Matrices are represented in standard mathematical notation:
 * The matrix M has R rows and C columns.
 * An element is represented by M[r,c].
 * Matrices are stored in column major order.
 *
 * Note that memory in C++ is allocated in row major form, thus the initialiser is transposed
 *
 *  {{ 1, 2 },  T   --> {{ 1, 3 },  // Col 0
 *   { 3, 4 }}           { 2, 4 }}  // Col 1
 *
 *  So that matrices may be post-multiplied.
 */

namespace zap { namespace maths {
    template <typename T>
    struct /*ALIGN_DECL(16)*/ mat2 {
        using type = T;

        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "mat2<T>: T must be an algebraic type");

        using row_t = vec2<T>;
        using col_t = vec2<T>;

        constexpr static size_t bytesize() { return sizeof(mat2<T>); }
        constexpr static size_t size() { return 4; }
        constexpr static size_t cols() { return 2; }
        constexpr static size_t rows() { return 2; }
        constexpr static size_t idx(size_t row, size_t col) { return col*rows() + row; }

        mat2() { }
        constexpr explicit mat2(T init) : m00(init), m10(init), m01(init), m11(init) { }
        constexpr mat2(const static_list<T, 4>& lst) : m00(lst[0]), m10(lst[2]), m01(lst[1]), m11(lst[3]) { }
        constexpr mat2(T m00, T m01, T m10, T m11) : m00(m00), m10(m10), m01(m01), m11(m11) { }
        constexpr explicit mat2(T m00, T m11) : m00(m00), m10(T(0)), m01(T(0)), m11(m11) { }
        constexpr explicit mat2(const col_t& d) : m00(d.x), m10(T(0)), m01(T(0)), m11(d.y) { }
        constexpr mat2(const mat2& rhs) : m00(rhs.m00), m10(rhs.m10), m01(rhs.m01), m11(rhs.m11) { }

        mat2& operator=(const mat2& rhs) {
            if(this != &rhs) std::copy(rhs.begin(), rhs.end(), begin());
            return *this;
        }

        constexpr static mat2 make_col(T m00, T m10, T m01, T m11) {
            return mat2(m00, m01, m10, m11);
        }

        constexpr static mat2 make_row(const row_t& row0, const row_t& row1) {
            return mat2(row0[0], row0[1], row1[0], row1[1]);
        }
        constexpr static mat2 make_col(const col_t& col0, const col_t& col1) {
            return mat2(col0[0], col1[0], col0[1], col1[1]);
        }

        constexpr static mat2 identity() {
            return mat2(T(1), T(1));
        }

        static mat2 make_rotation(T theta) {
            static_assert(std::is_floating_point<T>::value, ZERR_TYPE_FLOATING);
            const T cos = std::cos(theta), sin = std::sin(theta);
            return mat2(cos, -sin, sin, cos);
        }

        T* begin() { return arr; }
        T* end() { return arr + size(); }
        const T* begin() const { return arr; }
        const T* end() const { return arr + size(); }
        const T* data() const { return arr; }
        T* data() { return arr; }

        void clean() {
            for(auto& v : (*this)) {
                if(is_zero(v)) v = T(0);
            }
        }

        const T& operator[](size_t i) const {
            assert(i < size() && ZERR_IDX_OUT_OF_RANGE);
            return arr[i];
        }

        T& operator[](size_t i) {
            assert(i < size() && ZERR_IDX_OUT_OF_RANGE);
            return arr[i];
        }

        const T& operator()(size_t row, size_t col) const {
            assert(row < rows() && col < cols() && ZERR_IDX_OUT_OF_RANGE);
            return operator[](idx(row,col));
        }

        T& operator()(size_t row, size_t col) {
            assert(row < rows() && col < cols() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx(row,col)];
        }

        col_t col(size_t c) const {
            assert(c < cols() && ZERR_IDX_OUT_OF_RANGE);
            return col_t(operator()(0,c), operator()(1,c));
        }

        row_t row(size_t r) const {
            assert(r < rows() && ZERR_IDX_OUT_OF_RANGE);
            return row_t(operator()(r,0), operator()(r,1));
        }

        mat2& operator+=(const mat2& rhs);
        mat2& operator-=(const mat2& rhs);
        template <typename S> mat2& operator*=(S s);
        template <typename S> mat2& operator/=(S s);
        mat2& operator*=(const mat2& rhs);
        mat2& clear() { for(auto& e : arr) e = type(0); return *this; }

        mat2 inverse(type epsilon=std::numeric_limits<type>::epsilon()) const;

        mat2<T>& transpose() {
            auto& self = *this;
            std::swap(self(0,1), self(1,0));
            return self;
        }

        union {
            struct {
                T m00, m10,     // Column 0
                  m01, m11;     // Column 1
            };
            T arr[size()];
#ifdef ZAP_MATHS_SSE2
            __m128 xmm;
#endif
        };
	} /*ALIGN_ATTR(16)*/;

    template <typename T>
    mat2<T> make_rotation(T angle) {
        mat2<T> mat;
        T c_theta = cos(angle);
        T s_theta = sin(angle);
        mat(0,0) = +c_theta; mat(0,1) = -s_theta;
        mat(1,0) = +s_theta; mat(1,1) = +c_theta;
        return mat;
    }

    template <typename T>
    mat2<T>& mat2<T>::operator+=(const mat2<T>& rhs) {
        arr[0] += rhs[0]; arr[1] += rhs[1]; arr[2] += rhs[2]; arr[3] += rhs[3];
        return *this;
    }

    template <typename T>
    mat2<T>& mat2<T>::operator-=(const mat2<T>& rhs) {
        arr[0] -= rhs[0]; arr[1] -= rhs[1]; arr[2] -= rhs[2]; arr[3] -= rhs[3];
    }

    template <typename T>
    template <typename S>
    mat2<T>& mat2<T>::operator*=(S s) {
        for(auto& v : *this) v *= s;
        return *this;
    }

    template <typename T>
    template <typename S>
    mat2<T>& mat2<T>::operator/=(S s) {
        assert(!eq(s,0) && "s == 0");
        const S inv = S(1)/s;
        for(auto& v : *this) v *= inv;
    };

    template <typename T>
    mat2<T>& mat2<T>::operator*=(const mat2<T>& rhs) {
        mat2<T> lhs = *this;
        arr[0] = lhs(0,0)*rhs(0,0) + lhs(0,1)*rhs(1,0);
        arr[1] = lhs(0,0)*rhs(0,1) + lhs(0,1)*rhs(1,1);
        arr[2] = lhs(1,0)*rhs(0,0) + lhs(1,1)*rhs(1,0);
        arr[3] = lhs(1,0)*rhs(0,1) + lhs(1,1)*rhs(1,1);
        return *this;
    }

    template <typename T, typename S>
    constexpr mat2<T> operator*(const S& s, const mat2<T>& rhs) {
        return mat2<T>(s*rhs.m00, s*rhs.m01, s*rhs.m10, s*rhs.m11);
    }

    template <typename T, typename S>
    constexpr mat2<T> operator*(const mat2<T>& rhs, const S& s) {
        return operator*(s,rhs);
    };

    template <typename T, typename S>
    constexpr mat2<T> operator/(const mat2<T>& lhs, const S& s) {
        static_assert(s != S(0), "s == 0");
        constexpr S inv = S(1)/s;
        return mat2<T>(inv*lhs.m00, inv*lhs.m01, inv*lhs.m10, inv*lhs.m11);
    };

    template <typename T>
    constexpr mat2<T> operator+(const mat2<T>& lhs, const mat2<T>& rhs) {
        return mat2<T>(lhs.m00+rhs.m00, lhs.m01+rhs.m01, lhs.m10+rhs.m10, lhs.m11+rhs.m11);
    }

    template <typename T>
    constexpr mat2<T> operator-(const mat2<T>& lhs, const mat2<T>& rhs) {
        return mat2<T>(lhs.m00-rhs.m00, lhs.m01-rhs.m01, lhs.m10-rhs.m10, lhs.m11-rhs.m11);
    }

    template <typename T>
    constexpr mat2<T> operator*(const mat2<T>& lhs, const mat2<T>& rhs) {
        return mat2<T>(
                lhs.m00*rhs.m00 + lhs.m01*rhs.m10,
                lhs.m00*rhs.m01 + lhs.m01*rhs.m11,
                lhs.m10*rhs.m00 + lhs.m11*rhs.m10,
                lhs.m10*rhs.m01 + lhs.m11*rhs.m11
        );
    }

    template <typename T>
    vec2<T> operator*(const mat2<T>& m, const vec2<T>& v) {
        return vec2<T>(
                m(0,0)*v[0] + m(0,1)*v[1],
                m(1,0)*v[0] + m(1,1)*v[1]
        );
    }

    template <typename T>
    constexpr mat2<T> transpose(const mat2<T>& M) {
        return mat2<T>(M.m00, M.m10, M.m01, M.m11);
    }

    template <typename T>
    mat2<T> mat2<T>::inverse(typename mat2<T>::type epsilon) const {
        mat2<T> inv;
        T det = arr[0]*arr[3] - arr[1]*arr[2];
        if(std::abs(det) > epsilon) {
            T inv_det = type(1)/det;
            inv[0] = arr[3]*inv_det;  inv[1] = -arr[1]*inv_det;
            inv[2] = -arr[2]*inv_det; inv[3] = arr[0]*inv_det;
        } else {
            inv.clear();
        }

        return inv;
    }

}}

#endif //ZAP_MAT2_HPP

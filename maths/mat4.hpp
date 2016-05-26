//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_MAT4_HPP
#define ZAP_MAT4_HPP

#include "maths.hpp"
#include "vec4.hpp"

#include <iostream>


/* Note:
 * Matrices are represented in standard mathematical notation:
 * The matrix M has R rows and C columns.
 * An element is represented by M[r,c].
 * Matrices are stored in column major order.
 *
 * Note that memory in C++ is allocated in row major form, thus the initialiser is transposed
 *
 *  [ {{  0,  1,  2,   3 }, ] T   --> {{ 0,  4,  8, 12 },  // Col 0
 *  [  {  4,  5,  6,   7 }, ]          { 1,  5,  9, 13 },  // Col 1
 *  [  {  8,  9, 10,  11 }, ]          { 2,  6, 10, 14 },  // Col 2
 *  [  { 12, 13, 14,  15 }} ]          { 3,  7, 11, 15 }}  // Col 3
 *  So that matrices may be post-multiplied.
 *
 *  The matrix does full multiplication so it can be used as a 4x4 matrix or affine matrix.  An optimised 3x4 matrix
 *  should be added for affine transformations.
 */

namespace zap { namespace maths {
    template <typename T>
    struct ALIGN_DECL(16) mat4 {
        using type = T;

        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "mat4<T>: T must be an algebraic type");

        using row_t = vec4<T>;
        using col_t = vec4<T>;

        constexpr static size_t size() { return 16; }
        constexpr static size_t bytesize() { return sizeof(mat4<T>); }
        constexpr static size_t cols() { return 4; }
        constexpr static size_t rows() { return 4; }
        constexpr static size_t idx(size_t row, size_t col) { return col*rows() + row; }

        constexpr mat4() { }
        constexpr explicit mat4(T init) : m00(init), m10(init), m20(init), m30(init),
                                          m01(init), m11(init), m21(init), m31(init),
                                          m02(init), m12(init), m22(init), m32(init),
                                          m03(init), m13(init), m23(init), m33(init) { }
        constexpr mat4(const static_list<T, 16>& lst) : m00(lst[0]), m10(lst[4]), m20(lst[8]),  m30(lst[12]),
                                                        m01(lst[1]), m11(lst[5]), m21(lst[9]),  m31(lst[13]),
                                                        m02(lst[2]), m12(lst[6]), m22(lst[10]), m32(lst[14]),
                                                        m03(lst[3]), m13(lst[7]), m23(lst[11]), m33(lst[15]) { }
        constexpr mat4(T m00, T m01, T m02, T m03, T m10, T m11, T m12, T m13,
                       T m20, T m21, T m22, T m23, T m30, T m31, T m32, T m33) :
                                                     m00(m00), m10(m10), m20(m20), m30(m30),
                                                     m01(m01), m11(m11), m21(m21), m31(m31),
                                                     m02(m02), m12(m12), m22(m22), m32(m32),
                                                     m03(m03), m13(m13), m23(m23), m33(m33) { }
        constexpr mat4(T m00, T m11, T m22, T m33) : m00(m00),  m10(T(0)), m20(T(0)), m30(T(0)),
                                                     m01(T(0)), m11(m11),  m21(T(0)), m31(T(0)),
                                                     m02(T(0)), m12(T(0)), m22(m22),  m32(T(0)),
                                                     m03(T(0)), m13(T(0)), m23(T(0)), m33(m33) { }
        constexpr mat4(const col_t& col0, const col_t& col1, const col_t& col2, const col_t& col3) :
                m00(col0[0]), m10(col0[1]), m20(col0[2]), m30(col0[3]),
                m01(col1[0]), m11(col1[1]), m21(col1[2]), m31(col1[3]),
                m02(col2[0]), m12(col2[1]), m22(col2[2]), m32(col2[3]),
                m03(col3[0]), m13(col3[1]), m23(col3[2]), m33(col3[3]) { }

        constexpr static mat4 make_row(const row_t& row0, const row_t& row1, const row_t& row2, const row_t& row3) {
            return mat4(row0[0], row0[1], row0[2], row0[3], row1[0], row1[1], row1[2], row1[3],
                        row2[0], row2[1], row2[2], row2[3], row3[0], row3[1], row3[2], row3[3]);
        }
        constexpr static mat4 make_col(const col_t& col0, const col_t& col1, const col_t& col2, const col_t& col3) {
            return mat4(col0[0], col1[0], col2[0], col3[0], col0[1], col1[1], col2[1], col3[1],
                        col0[2], col1[2], col2[2], col3[2], col0[3], col1[3], col2[3], col3[3]);
        }

        constexpr static mat4 identity() {
            return mat4(1,1,1,1);
        }

        const T& operator()(size_t row, size_t col) const {
            assert(row < rows() && col < cols() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx(row,col)];
        }

        T& operator()(size_t row, size_t col) {
            assert(row < rows() && col < cols() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx(row,col)];
        }

        T& operator[](size_t idx) {
            assert(idx < size() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx];
        }

        const T& operator[](size_t idx) const {
            assert(idx < size() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx];
        }

        inline T* begin() { return arr; }
        inline T* end() { return arr + size(); }
        inline const T* begin() const { return arr; }
        inline const T* end() const { return arr + size(); }
        inline const T* data() const { return arr; }
        inline T* data() { return arr; }

        mat4<T>& operator+=(const mat4& rhs) { for(size_t i = 0; i < size(); ++i) (*this)[i] += rhs[i]; }
        mat4<T>& operator-=(const mat4& rhs) { for(size_t i = 0; i < size(); ++i) (*this)[i] -= rhs[i]; }
        template <typename S> mat4<T>& operator*=(const S& scalar) { for(auto& v : *this) v *= scalar; return *this; }

        mat4<T>& operator*=(const mat4& rhs) {
            auto& self = *this; mat4<T> lhs(self);
            self(0,0) = lhs(0,0)*rhs(0,0) + lhs(0,1)*rhs(1,0) + lhs(0,2)*rhs(2,0) + lhs(0,3)*rhs(3,0);
            self(0,1) = lhs(0,0)*rhs(0,1) + lhs(0,1)*rhs(1,1) + lhs(0,2)*rhs(2,1) + lhs(0,3)*rhs(3,1);
            self(0,2) = lhs(0,0)*rhs(0,2) + lhs(0,1)*rhs(1,2) + lhs(0,2)*rhs(2,2) + lhs(0,3)*rhs(3,2);
            self(0,3) = lhs(0,0)*rhs(0,3) + lhs(0,1)*rhs(1,3) + lhs(0,2)*rhs(2,3) + lhs(0,3)*rhs(3,3);
            self(1,0) = lhs(1,0)*rhs(0,0) + lhs(1,1)*rhs(1,0) + lhs(1,2)*rhs(2,0) + lhs(1,3)*rhs(3,0);
            self(1,1) = lhs(1,0)*rhs(0,1) + lhs(1,1)*rhs(1,1) + lhs(1,2)*rhs(2,1) + lhs(1,3)*rhs(3,1);
            self(1,2) = lhs(1,0)*rhs(0,2) + lhs(1,1)*rhs(1,2) + lhs(1,2)*rhs(2,2) + lhs(1,3)*rhs(3,2);
            self(1,3) = lhs(1,0)*rhs(0,3) + lhs(1,1)*rhs(1,3) + lhs(1,2)*rhs(2,3) + lhs(1,3)*rhs(3,3);
            self(2,0) = lhs(2,0)*rhs(0,0) + lhs(2,1)*rhs(1,0) + lhs(2,2)*rhs(2,0) + lhs(2,3)*rhs(3,0);
            self(2,1) = lhs(2,0)*rhs(0,1) + lhs(2,1)*rhs(1,1) + lhs(2,2)*rhs(2,1) + lhs(2,3)*rhs(3,1);
            self(2,2) = lhs(2,0)*rhs(0,2) + lhs(2,1)*rhs(1,2) + lhs(2,2)*rhs(2,2) + lhs(2,3)*rhs(3,2);
            self(2,3) = lhs(2,0)*rhs(0,3) + lhs(2,1)*rhs(1,3) + lhs(2,2)*rhs(2,3) + lhs(2,3)*rhs(3,3);
            self(3,0) = lhs(3,0)*rhs(0,0) + lhs(3,1)*rhs(1,0) + lhs(3,2)*rhs(2,0) + lhs(3,3)*rhs(3,0);
            self(3,1) = lhs(3,0)*rhs(0,1) + lhs(3,1)*rhs(1,1) + lhs(3,2)*rhs(2,1) + lhs(3,3)*rhs(3,1);
            self(3,2) = lhs(3,0)*rhs(0,2) + lhs(3,1)*rhs(1,2) + lhs(3,2)*rhs(2,2) + lhs(3,3)*rhs(3,2);
            self(3,3) = lhs(3,0)*rhs(0,3) + lhs(3,1)*rhs(1,3) + lhs(3,2)*rhs(2,3) + lhs(3,3)*rhs(3,3);
        }

        mat4<T>& transpose() {
            auto& self = *this;
            std::swap(self(0,1), self(1,0)); std::swap(self(0,2),self(2,0)); std::swap(self(0,3),self(3,0));
            std::swap(self(1,2), self(2,1)); std::swap(self(1,3),self(3,1)); std::swap(self(2,3),self(3,2));
            return self;
        }

        union {
            struct {
                T m00, m10, m20, m30,     // Column 0
                  m01, m11, m21, m31,     // Column 1
                  m02, m12, m22, m32,     // Column 2
                  m03, m13, m23, m33;     // Column 3
            };
            T arr[size()];
#ifdef ZAP_MATHS_SSE2
            __m128 xmm[cols()];
#endif //ZAP_MATHS_SSE2
        };
	} ALIGN_ATTR(16);

    template <typename T>
    mat4<T> make_translation(T x, T y, T z) {
        mat4<T> r = mat4<T>::identity();
        r(0,3) = x; r(1,3) = y; r(2,3) = z;
        return r;
    }

    template <typename T, typename S>
    mat4<T> make_rotation(const vec3<T>& axis, const S& theta) {
        assert(axis.is_unit() && "Rotation axis must be normalised");
        mat4<T> r = mat4<T>::identity();
        T ct = std::cos(theta), st = std::sin(theta), t = 1 - ct;
        T tx = t*axis.x,   ty = t*axis.y,   tz = t*axis.z;
        T sx = st*axis.x,  sy = st*axis.y,  sz = st*axis.z;
        T txy = tx*axis.y, tyz = ty*axis.z, txz = tx*axis.z;

        r(0,0) = tx*axis.x + ct; r(0,1) = txy + sz;       r(0,2) = txz - sy;
        r(1,0) = txy - sz;       r(1,1) = ty*axis.y + ct; r(1,2) = tyz + sx;
        r(2,0) = txz + sy;       r(2,1) = tyz - sx;       r(2,2) = tz*axis.z + ct;
        return r;
    };


    template <typename T>
    mat4<T> make_perspective(T fov, T ar, T dmin, T dmax) {
        mat4<T> r;
        T ha_rad = deg_to_rad<T>(.5 * fov);
        T umax = dmin * std::tan(ha_rad), rmax = ar * umax, umin = -umax, rmin = -rmax;
        T inv_d = 1/(dmax - dmin), inv_u = 1/(umax - umin), inv_r = 1/(rmax - rmin);
        T sD = (dmin + dmax) * inv_d, sU = (umin + umax) * inv_u, sR = (rmin + rmax) * inv_r;
        T two_dmin_inv_r = 2 * dmin * inv_r;
        T two_dmin_inv_u = 2 * dmin * inv_u;
        T two_dmin_dmax_inv_D = 2*(dmin * (dmax * inv_d));

        // Col 1, 2, 3, 4
        r(0,0) = two_dmin_inv_r; r(1,0) = 0; r(2,0) = 0; r(3,0) = 0;
        r(0,1) = 0; r(1,1) = two_dmin_inv_u; r(2,1) = 0; r(3,1) = 0;
        r(0,2) = -sR; r(1,2) = -sU; r(2,2) = sD; r(3,2) = T(1);
        r(0,3) = 0; r(1,3) = 0; r(2,3) = -two_dmin_dmax_inv_D; r(3,3) = 0;
        return r;
    }

    template <typename T>
    mat4<T> operator+(const mat4<T>& lhs, const mat4<T>& rhs) {
        mat4<T> r;
        for(size_t i = 0; i != mat4<T>::size(); ++i) r[i] = lhs[i] + rhs[i];
        return r;
    }

    template <typename T>
    mat4<T> operator-(const mat4<T>& lhs, const mat4<T>& rhs) {
        mat4<T> r;
        for(size_t i = 0; i != mat4<T>::size(); ++i) r[i] = lhs[i] - rhs[i];
        return r;
    }

    template <typename T, typename S>
    mat4<T> operator*(const mat4<T>& lhs, const S& scalar) {
        mat4<T> r;
        for(size_t i = 0; i != mat4<T>::size(); ++i) r[i] = scalar*lhs[i];
        return r;
    };

    template <typename T, typename S>
    mat4<T> operator*(const S& scalar, const mat4<T>& rhs) {
        mat4<T> r;
        for(size_t i = 0; i != mat4<T>::size(); ++i) r[i] = scalar*rhs[i];
        return r;
    };

    template <typename T>
    mat4<T> operator*(const mat4<T>& lhs, const mat4<T>& rhs) {
        mat4<T> r;
        r(0,0) = lhs(0,0)*rhs(0,0) + lhs(0,1)*rhs(1,0) + lhs(0,2)*rhs(2,0) + lhs(0,3)*rhs(3,0);
        r(0,1) = lhs(0,0)*rhs(0,1) + lhs(0,1)*rhs(1,1) + lhs(0,2)*rhs(2,1) + lhs(0,3)*rhs(3,1);
        r(0,2) = lhs(0,0)*rhs(0,2) + lhs(0,1)*rhs(1,2) + lhs(0,2)*rhs(2,2) + lhs(0,3)*rhs(3,2);
        r(0,3) = lhs(0,0)*rhs(0,3) + lhs(0,1)*rhs(1,3) + lhs(0,2)*rhs(2,3) + lhs(0,3)*rhs(3,3);
        r(1,0) = lhs(1,0)*rhs(0,0) + lhs(1,1)*rhs(1,0) + lhs(1,2)*rhs(2,0) + lhs(1,3)*rhs(3,0);
        r(1,1) = lhs(1,0)*rhs(0,1) + lhs(1,1)*rhs(1,1) + lhs(1,2)*rhs(2,1) + lhs(1,3)*rhs(3,1);
        r(1,2) = lhs(1,0)*rhs(0,2) + lhs(1,1)*rhs(1,2) + lhs(1,2)*rhs(2,2) + lhs(1,3)*rhs(3,2);
        r(1,3) = lhs(1,0)*rhs(0,3) + lhs(1,1)*rhs(1,3) + lhs(1,2)*rhs(2,3) + lhs(1,3)*rhs(3,3);
        r(2,0) = lhs(2,0)*rhs(0,0) + lhs(2,1)*rhs(1,0) + lhs(2,2)*rhs(2,0) + lhs(2,3)*rhs(3,0);
        r(2,1) = lhs(2,0)*rhs(0,1) + lhs(2,1)*rhs(1,1) + lhs(2,2)*rhs(2,1) + lhs(2,3)*rhs(3,1);
        r(2,2) = lhs(2,0)*rhs(0,2) + lhs(2,1)*rhs(1,2) + lhs(2,2)*rhs(2,2) + lhs(2,3)*rhs(3,2);
        r(2,3) = lhs(2,0)*rhs(0,3) + lhs(2,1)*rhs(1,3) + lhs(2,2)*rhs(2,3) + lhs(2,3)*rhs(3,3);
        r(3,0) = lhs(3,0)*rhs(0,0) + lhs(3,1)*rhs(1,0) + lhs(3,2)*rhs(2,0) + lhs(3,3)*rhs(3,0);
        r(3,1) = lhs(3,0)*rhs(0,1) + lhs(3,1)*rhs(1,1) + lhs(3,2)*rhs(2,1) + lhs(3,3)*rhs(3,1);
        r(3,2) = lhs(3,0)*rhs(0,2) + lhs(3,1)*rhs(1,2) + lhs(3,2)*rhs(2,2) + lhs(3,3)*rhs(3,2);
        r(3,3) = lhs(3,0)*rhs(0,3) + lhs(3,1)*rhs(1,3) + lhs(3,2)*rhs(2,3) + lhs(3,3)*rhs(3,3);
        return r;
    }

    using mat4b = mat4<uint8_t>;
    using mat4s = mat4<int16_t>;
    using mat4i = mat4<int32_t>;
    using mat4f = mat4<float>;
    using mat4d = mat4<double>;
}}
#endif //ZAP_MAT4_HPP

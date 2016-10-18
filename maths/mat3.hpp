//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_MAT3_HPP
#define ZAP_MAT3_HPP

#include "maths.hpp"
#include "vec2.hpp"
#include "vec3.hpp"
#include "mat2.hpp"

/* Note:
 * Matrices are represented in standard mathematical notation:
 * The matrix M has R rows and C columns.
 * An element is represented by M[r,c].
 * Matrices are stored in column major order.
 *
 * Note that memory in C++ is allocated in row major form, thus the initialiser is transposed
 *
 *  [ {{ 0, 1, 2 }, ] T   --> {{ 0, 3, 6 },  // Col 0
 *  [  { 3, 4, 5 }, ]          { 1, 4, 7 },  // Col 1
 *  [  { 6, 7, 8 }} ]          { 2, 5, 8 }}  // Col 2
 *  So that matrices may be post-multiplied.
 *
 *  Note that the mat3 structure is aligned to 3 __m128 registers and therefore wastes
 *  3 floats as a 3 x 4 matrix.
 */

namespace zap { namespace maths {
    template <typename T>
    struct mat3 {
        using type = T;

        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "mat3<T>: T must be an algebraic type");

        using row_t = vec3<T>;
        using col_t = vec3<T>;
        using vec_t = vec2<T>;
        using rot_t = mat2<T>;

        constexpr static size_t size() { return 9; }
        constexpr static size_t bytesize() { return sizeof(mat3<T>); }
        constexpr static size_t cols() { return 3; }
        constexpr static size_t rows() { return 3; }
        constexpr static size_t idx(size_t row, size_t col) { return col*rows() + row; }

        mat3() { }
        constexpr explicit mat3(T init) : m00(init), m10(init), m20(init),
                                          m01(init), m11(init), m21(init),
                                          m02(init), m12(init), m22(init) { }
        constexpr mat3(const static_list<T, 9>& lst) : m00(lst[0]), m10(lst[3]), m20(lst[6]),
                                                       m01(lst[1]), m11(lst[4]), m21(lst[7]),
                                                       m02(lst[2]), m12(lst[5]), m22(lst[8]) { }
        constexpr mat3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22) : m00(m00), m10(m10), m20(m20),
                                                                                        m01(m01), m11(m11), m21(m21),
                                                                                        m02(m02), m12(m12), m22(m22) { }
        constexpr mat3(T m00, T m11, T m22) : m00(m00),  m10(T(0)), m20(T(0)),
                                              m01(T(0)), m11(m11),  m21(T(0)),
                                              m02(T(0)), m12(T(0)), m22(m22) { }
        constexpr mat3(const col_t& d) : m00(d.x),  m10(T(0)), m20(T(0)),
                                         m01(T(0)), m11(d.y),  m21(T(0)),
                                         m02(T(0)), m12(T(0)), m22(d.z) { }
        constexpr mat3(const col_t& col0, const col_t& col1, const col_t& col2) :
                m00(col0[0]), m10(col0[1]), m20(col0[2]),
                m01(col1[0]), m11(col1[1]), m21(col1[2]),
                m02(col2[0]), m12(col2[1]), m22(col2[2]) { }
        constexpr mat3(const mat3& rhs) : m00(rhs.m00), m10(rhs.m10), m20(rhs.m20),
                                          m01(rhs.m01), m11(rhs.m11), m21(rhs.m21),
                                          m02(rhs.m02), m12(rhs.m12), m22(rhs.m22) { }
        mat3& operator=(const mat3& rhs) {
            if(this != &rhs) {
                m00 = rhs.m00; m10 = rhs.m10; m20 = rhs.m20;
                m01 = rhs.m01; m11 = rhs.m11; m21 = rhs.m21;
                m02 = rhs.m02; m12 = rhs.m12; m22 = rhs.m22;
            }
            return *this;
        }

        constexpr static mat3 make_row(const row_t& row0, const row_t& row1, const row_t& row2) {
            return mat3(row0[0], row0[1], row0[2], row1[0], row1[1], row1[2], row2[0], row2[1], row2[2]);
        }
        constexpr static mat3 make_col(const col_t& col0, const col_t& col1, const col_t& col2) {
            return mat3(col0[0], col1[0], col2[0], col0[1], col1[1], col2[1], col0[2], col1[2], col2[2]);
        }

        mat3& frame(const vec_t& r, const vec_t& u, const vec_t& P) {
            column(0,r); column(1,u); column(2,P);
            return *this;
        }

        constexpr static mat3 identity() {
            return mat3(T(1),T(1),T(1));
        }

        // Need to write a custom iterator for dealing with the 16 byte alignment

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

        vec3<T> col3(size_t col) const {
            checkidx(col, cols());
            return vec3<T>(operator()(0,col), operator()(1,col), operator()(2,col));
        }
        vec2<T> col2(size_t col) const {
            checkidx(col, cols());
            return vec2<T>(operator()(0,col), operator()(1,col));
        }
        vec2<T> col(size_t c) const { return col2(c); }
        void column(size_t col, const vec3<T>& v) {
            checkidx(col, cols());
            assert( ( ((col < 2) && eq(v.z, T(0))) || ((col == 2) && eq(v.z, T(1))) ) && "0..1 must be vectors, 2 must be a point");
            operator()(0,col) = v[0]; operator()(1,col) = v[1]; operator()(2,col) = v[2];
        }
        void column(size_t col, const vec2<T>& v) {
            checkidx(col, cols());
            operator()(0,col) = v[0]; operator()(1,col) = v[1];
        }
        vec3<T> row3(size_t row) const {
            checkidx(row, rows());
            return vec3<T>(operator()(row,0), operator()(row,1), operator()(row,2));
        }
        vec2<T> row2(size_t row) const {
            checkidx(row, rows());
            return vec2<T>(operator()(row,0), operator()(row,1));
        }
        vec2<T> row(size_t r) const { return row2(r); }
        void row(size_t row, const vec3<T>& v) {
            checkidx(row, rows());
            operator()(row,0) = v[0]; operator()(row,1) = v[1]; operator()(row,2) = v[2];
        }
        void row(size_t row, const vec2<T>& v) {
            checkidx(row, rows());
            operator()(row,0) = v[0]; operator()(row,1) = v[1];
        }
        void rotation(const mat2<T>& R) {
            for(size_t c = 0, cend = cols()-1; c != cend; ++c) {
                for(size_t r = 0, rend = rows()-1; r != rend; ++r) {
                    operator()(r,c) = R(r,c);
                }
            }
        }

        mat3<T>& operator+=(const mat3& rhs) { for(int i = 0; i != size(); ++i) (*this)[i] += rhs[i]; }
        mat3<T>& operator-=(const mat3& rhs) { for(int i = 0; i != size(); ++i) (*this)[i] += rhs[i]; }

        mat3& clear() { for(auto& e : arr) e = type(0); return *this; }
        mat3 inverse(type epsilon=std::numeric_limits<type>::epsilon()) const;
        vec2<T> transform(const vec2<T>& v) const;

        mat3<T>& transpose() {
            auto& self = *this;
            std::swap(self(0,1), self(1,0)); std::swap(self(0,2), self(2,0)); std::swap(self(1,2),self(2,1));
            return self;
        }

        union {
            struct {
                T m00, m10, m20,     // Column 0
                  m01, m11, m21,     // Column 1
                  m02, m12, m22;     // Column 2
            };
            T arr[cols()*rows()];
#ifdef ZAP_MATHS_SSE2
            __m128 xmm[cols()];
#endif //ZAP_MATHS_SSE2
        };
	};

    template <typename T>
    mat3<T> make_scale(const T& x, const T& y) {
        return mat3<T>(x, y, T(1));
    }

    template <typename T>
    mat3<T> make_translation(const T& x, const T& y) {
        return mat3<T>(T(1), T(0), x,
                       T(0), T(1), y,
                       T(0), T(0), T(1));
    }

    template <typename T>
    vec2<T> mat3<T>::transform(const vec2<T>& v) const {
        return vec2<T>(
                arr[idx(0,0)]*v[0] + arr[idx(0,1)]*v[1] + arr[idx(0,2)],
                arr[idx(1,0)]*v[0] + arr[idx(1,1)]*v[1] + arr[idx(1,2)]
        );
    }

    template <typename T>
    mat3<T> mat3<T>::inverse(T epsilon) const {
        mat3 inv;

        inv[0] = arr[4]*arr[8] - arr[5]*arr[7]; inv[1] = arr[2]*arr[7] - arr[1]*arr[8]; inv[2] = arr[1]*arr[5] - arr[2]*arr[4];
        inv[3] = arr[5]*arr[6] - arr[3]*arr[8]; inv[4] = arr[0]*arr[8] - arr[2]*arr[6]; inv[5] = arr[2]*arr[3] - arr[0]*arr[5];
        inv[6] = arr[3]*arr[7] - arr[4]*arr[6]; inv[7] = arr[1]*arr[6] - arr[0]*arr[7]; inv[8] = arr[0]*arr[4] - arr[1]*arr[3];

        T det = arr[0]*inv[0] + arr[1]*inv[3] + arr[2]*inv[6];

        if(std::abs(det) > epsilon) {
            T inv_det = ((T)1)/det;
            inv[0] *= inv_det; inv[1] *= inv_det; inv[2] *= inv_det;
            inv[3] *= inv_det; inv[4] *= inv_det; inv[5] *= inv_det;
            inv[6] *= inv_det; inv[7] *= inv_det; inv[8] *= inv_det;
            return inv;
        }

        return mat3((T)0);
    }

    template <typename T>
    mat3<T> operator*(const mat3<T>& m, const mat3<T>& n) {
        mat3<T> r;
        r(0,0) = m(0,0)*n(0,0) + m(0,1)*n(1,0) + m(0,2)*n(2,0);
        r(0,1) = m(0,0)*n(0,1) + m(0,1)*n(1,1) + m(0,2)*n(2,1);
        r(0,2) = m(0,0)*n(0,2) + m(0,1)*n(1,2) + m(0,2)*n(2,2);
        r(1,0) = m(1,0)*n(0,0) + m(1,1)*n(1,0) + m(1,2)*n(2,0);
        r(1,1) = m(1,0)*n(0,1) + m(1,1)*n(1,1) + m(1,2)*n(2,1);
        r(1,2) = m(1,0)*n(0,2) + m(1,1)*n(1,2) + m(1,2)*n(2,2);
        r(2,0) = m(2,0)*n(0,0) + m(2,1)*n(1,0) + m(2,2)*n(2,0);
        r(2,1) = m(2,0)*n(0,1) + m(2,1)*n(1,1) + m(2,2)*n(2,1);
        r(2,2) = m(2,0)*n(0,2) + m(2,1)*n(1,2) + m(2,2)*n(2,2);
        return r;
    }

    template <typename T>
    vec2<T> operator*(const mat3<T>& m, const vec2<T>& v) {
        return vec2<T>(
                m(0,0)*v[0] + m(0,1)*v[1],
                m(1,0)*v[0] + m(1,1)*v[1]
        );
    }

    template <typename T>
    vec3<T> operator*(const mat3<T>& m, const vec3<T>& v) {
        return vec3<T>(
                m(0,0)*v[0] + m(0,1)*v[1] + m(0,2)*v[2],
                m(1,0)*v[0] + m(1,1)*v[1] + m(1,2)*v[2],
                m(2,0)*v[0] + m(2,1)*v[1] + m(2,2)*v[2]
        );
    }

    template <typename T>
    mat3<T> transpose(const mat3<T>& M) {
        auto R = M;
        std::swap(R(0,1), R(1,0)); std::swap(R(0,2),R(2,0)); std::swap(R(1,2), R(2,1));
        return R;
    }

    using mat3b = mat3<uint8_t>;
    using mat3s = mat3<int16_t>;
    using mat3i = mat3<int32_t>;
    using mat3f = mat3<float>;
    using mat3d = mat3<double>;
}}

#endif //ZAP_MAT3_HPP

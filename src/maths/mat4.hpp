//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_MAT4_HPP
#define ZAP_MAT4_HPP

#include <maths/maths.hpp>
#include <maths/vec3.hpp>
#include <maths/vec4.hpp>

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
 *  So that matrices may be efficiently post-multiplied.
 *
 *  The matrix does full multiplication so it can be used as a 4x4 matrix or affine matrix.  An optimised 3x4 matrix
 *  should be added for affine transformations.
 */

namespace zap { namespace maths {
    template <typename T>
    struct mat4 {
        using type = T;

        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "mat4<T>: T must be an algebraic type");

        using row_t = vec4<T>;
        using col_t = vec4<T>;
        using vec_t = vec3<T>;
        using rot_t = mat3<T>;  // The rotation matrix type

        constexpr static size_t size() { return 16; }
        constexpr static size_t bytesize() { return sizeof(mat4<T>); }
        constexpr static size_t cols() { return 4; }
        constexpr static size_t rows() { return 4; }
        constexpr static size_t idx(size_t row, size_t col) { return col*rows() + row; }

        mat4() = default;
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
        constexpr mat4(const col_t& d) : m00(d.x),  m10(T(0)), m20(T(0)), m30(T(0)),
                                         m01(T(0)), m11(d.y),  m21(T(0)), m31(T(0)),
                                         m02(T(0)), m12(T(0)), m22(d.z),  m32(T(0)),
                                         m03(T(0)), m13(T(0)), m23(T(0)), m33(d.w) { }
        constexpr mat4(const col_t& col0, const col_t& col1, const col_t& col2, const col_t& col3) :
                m00(col0[0]), m10(col0[1]), m20(col0[2]), m30(col0[3]),
                m01(col1[0]), m11(col1[1]), m21(col1[2]), m31(col1[3]),
                m02(col2[0]), m12(col2[1]), m22(col2[2]), m32(col2[3]),
                m03(col3[0]), m13(col3[1]), m23(col3[2]), m33(col3[3]) { }
        constexpr mat4(const vec_t& col0, const vec_t& col1, const vec_t& col2, const vec_t& col3) :
                m00(col0[0]), m10(col0[1]), m20(col0[2]), m30(T(0)),
                m01(col1[0]), m11(col1[1]), m21(col1[2]), m31(T(0)),
                m02(col2[0]), m12(col2[1]), m22(col2[2]), m32(T(0)),
                m03(col3[0]), m13(col3[1]), m23(col3[2]), m33(T(1)) { }
        constexpr mat4(const mat4& rhs) : m00(rhs.m00), m10(rhs.m10), m20(rhs.m20), m30(rhs.m30),
                                          m01(rhs.m01), m11(rhs.m11), m21(rhs.m21), m31(rhs.m31),
                                          m02(rhs.m02), m12(rhs.m12), m22(rhs.m22), m32(rhs.m32),
                                          m03(rhs.m03), m13(rhs.m13), m23(rhs.m23), m33(rhs.m33) { }

        mat4& operator=(const mat4& rhs) {
			if (this != &rhs) for(int i = 0; i != size(); ++i) arr[i] = rhs[i];
            return *this;
        }

        constexpr static mat4 make_row(const row_t& row0, const row_t& row1, const row_t& row2, const row_t& row3) {
            return mat4(row0[0], row0[1], row0[2], row0[3], row1[0], row1[1], row1[2], row1[3],
                        row2[0], row2[1], row2[2], row2[3], row3[0], row3[1], row3[2], row3[3]);
        }
        constexpr static mat4 make_col(const col_t& col0, const col_t& col1, const col_t& col2, const col_t& col3) {
            return mat4(col0[0], col1[0], col2[0], col3[0], col0[1], col1[1], col2[1], col3[1],
                        col0[2], col1[2], col2[2], col3[2], col0[3], col1[3], col2[3], col3[3]);
        }

        mat4& frame(const vec_t& r, const vec_t& u, const vec_t& d, const vec_t& P) {
            column(0,r); column(1,u); column(2,d); column(3,P);
            return *this;
        }

        constexpr static mat4 identity() {
            return mat4(T(1),T(1),T(1),T(1));
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

        vec4<T> col4(size_t col) const {
            checkidx(col, cols());
            return vec4<T>(operator()(0,col), operator()(1,col), operator()(2,col), operator()(3,col));
        }
        vec3<T> col3(size_t col) const {
            checkidx(col, cols());
            return vec3<T>(operator()(0,col), operator()(1,col), operator()(2,col));
        }
        vec3<T> col(size_t c) const { return col3(c); }
        void column(size_t col, const vec4<T>& v) {
            checkidx(col, cols());
            assert( ( ((col < 3) && eq(v.w, T(0))) || ((col == 3) && eq(v.w, T(1))) ) && "0..2 must be vectors, 3 must be a point");
            operator()(0,col) = v[0]; operator()(1,col) = v[1]; operator()(2,col) = v[2]; operator()(3,col) = v[3];
        }
        void column(size_t col, const vec3<T>& v) {
            checkidx(col, cols());
            operator()(0,col) = v[0]; operator()(1,col) = v[1]; operator()(2,col) = v[2];
        }
        vec4<T> row4(size_t row) const {
            checkidx(row, rows());
            return vec4<T>(operator()(row,0), operator()(row,1), operator()(row,2), operator()(row,3));
        }
        vec3<T> row3(size_t row) const {
            checkidx(row, rows());
            return vec3<T>(operator()(row,0), operator()(row,1), operator()(row,2));
        }
        vec3<T> row(size_t r) const { return row3(r); }
        void row(size_t row, const vec4<T>& v) {
            checkidx(row, rows());
            operator()(row,0) = v[0]; operator()(row,1) = v[1]; operator()(row,2) = v[2]; operator()(row,3) = v[3];
        }
        void row(size_t row, const vec3<T>& v) {
            checkidx(row, rows());
            operator()(row,0) = v[0]; operator()(row,1) = v[1]; operator()(row,2) = v[2];
        }
        void rotation(const mat3<T>& R) {
            for(size_t c = 0, cend = cols()-1; c != cend; ++c) {
                for(size_t r = 0, rend = rows()-1; r != rend; ++r) {
                    operator()(r,c) = R(r,c);
                }
            }
        }
        mat3<T> rotation() const {
            mat3<T> R;
            for(size_t c = 0, cend = cols()-1; c != cend; ++c) {
                for(size_t r = 0, rend = rows()-1; r != rend; ++r) {
                    R(r,c) = operator()(r,c);
                }
            }
            return R;
        }

        mat4<T>& operator+=(const mat4& rhs) { for(size_t i = 0; i != size(); ++i) (*this)[i] += rhs[i]; }
        mat4<T>& operator-=(const mat4& rhs) { for(size_t i = 0; i != size(); ++i) (*this)[i] -= rhs[i]; }
        template <typename S> mat4<T>& operator*=(const S& scalar) { for(auto& v : *this) v *= scalar; return *this; }

        // Not a transform, vector multiplication
        vec3<T> operator*(const vec3<T>& v) {
            vec3<T> r;
            r[0] = arr[idx(0,0)]*v[0] + arr[idx(0,1)]*v[1] + arr[idx(0,2)]*v[2];
            r[1] = arr[idx(1,0)]*v[0] + arr[idx(1,1)]*v[1] + arr[idx(1,2)]*v[2];
            r[2] = arr[idx(2,0)]*v[0] + arr[idx(2,1)]*v[1] + arr[idx(2,2)]*v[2];
            return r;
        }

        // This is a transform
        vec4<T> operator*(const vec4<T>& v) {
            vec4<T> r;
            r[0] = arr[idx(0,0)]*v[0] + arr[idx(0,1)]*v[1] + arr[idx(0,2)]*v[2] + arr[idx(0,3)]*v[3];
            r[1] = arr[idx(1,0)]*v[0] + arr[idx(1,1)]*v[1] + arr[idx(1,2)]*v[2] + arr[idx(1,3)]*v[3];
            r[2] = arr[idx(2,0)]*v[0] + arr[idx(2,1)]*v[1] + arr[idx(2,2)]*v[2] + arr[idx(2,3)]*v[3];
            r[3] = arr[idx(3,0)]*v[0] + arr[idx(3,1)]*v[1] + arr[idx(3,2)]*v[2] + arr[idx(3,3)]*v[3];
            return r;
        }

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
            return *this;
        }

        mat4& clear() { for(auto& e : arr) e = type(0); return *this; }
        mat4 inverse(type epsilon=std::numeric_limits<type>::epsilon()) const;
        vec3<T> transform(const vec3<T>& P) const;

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
	} /*ALIGN_ATTR(16)*/;

    template <typename T>
    vec3<T> mat4<T>::transform(const vec3<T>& P) const {
        vec3<T> R;
        R.x = arr[idx(0,0)]*P.x + arr[idx(0,1)]*P.y + arr[idx(0,2)]*P.z + arr[idx(0,3)];
        R.y = arr[idx(1,0)]*P.x + arr[idx(1,1)]*P.y + arr[idx(1,2)]*P.z + arr[idx(1,3)];
        R.z = arr[idx(2,0)]*P.x + arr[idx(2,1)]*P.y + arr[idx(2,2)]*P.z + arr[idx(2,3)];
        return R;
    }

    template <typename T>
    mat4<T> make_translation(T x, T y, T z) {
        mat4<T> r = mat4<T>::identity();
        r(0,3) = x; r(1,3) = y; r(2,3) = z;
        return r;
    }

    template <typename T>
    mat4<T> make_scale(T x, T y, T z) {
        mat4<T> r = mat4<T>::identity();
        r(0,0) = x; r(1,1) = y; r(2,2) = z;
        return r;
    }

    template <typename T, typename S>
    mat4<T> make_rotation(const vec3<T>& axis, const S& theta) {
        assert(axis.is_unit() && "Rotation axis must be normalised");
        mat4<T> r;
        T ct = std::cos(theta), st = std::sin(theta), t = 1 - ct;
        T tx = t*axis.x,   ty = t*axis.y,   tz = t*axis.z;
        T sx = st*axis.x,  sy = st*axis.y,  sz = st*axis.z;
        T txy = tx*axis.y, tyz = ty*axis.z, txz = tx*axis.z;

        r(0,0) = tx*axis.x + ct; r(0,1) = txy + sz;       r(0,2) = txz - sy;       r(0,3) = 0;
        r(1,0) = txy - sz;       r(1,1) = ty*axis.y + ct; r(1,2) = tyz + sx;       r(1,3) = 0;
        r(2,0) = txz + sy;       r(2,1) = tyz - sx;       r(2,2) = tz*axis.z + ct; r(2,3) = 0;
        r(3,0) = 0;              r(3,1) = 0;              r(3,2) = 0;              r(3,3) = T(1);
        return r;
    };

    template <typename T>
    mat4<T> make_frame(const vec3<T>& d, const vec3<T>& u, const vec3<T>& P) {
        auto r = normalise(cross(d, u));
        return mat4<T>(r, u, d, P);
    }

    // Takes degrees - force of habit
    template <typename T>
    mat4<T> make_perspective(T fov, T ar, T d_min, T d_max) {
        const T ha_rad_tan = std::tan(deg_to_rad(T(.5) * fov));
        mat4<T> P(0);
        P(0,0) = T(1) / (ar * ha_rad_tan);
        P(1,1) = T(1) / ha_rad_tan;
        P(2,2) = d_max / (d_min - d_max);
        P(2,3) = -(d_min * d_max) / (d_max - d_min);
        P(3,2) = T(-1);
        return P;
    }

    template <typename T>
    mat4<T> make_ortho(T r_min, T r_max, T u_min, T u_max, T d_min, T d_max) {
        mat4<T> P(1,1,1,1);
        P(0,0) = T(2) / (r_max - r_min);
        P(1,1) = T(2) / (u_max - u_min);
        P(2,2) = T(2) / (d_max - d_min);
        P(0,3) = -(r_max + r_min) / (r_max - r_min);
        P(1,3) = -(u_max + u_min) / (u_max - u_min);
        P(2,3) = -(d_max + d_min) / (d_max - d_min);
        return P;
    }

    template <typename T>
    mat4<T> orthogonolise(const vec3<T>& d, const vec3<T>& world_up=vec3<T>{T(0.), T(1.), T(0.)}) {
        assert(maths::eq(d.length_sqr(), 1.f, 8*std::numeric_limits<T>::epsilon()) && "Direction vector must be unit length");
        auto up = normalise(world_up - dot(world_up, d)*d);
        mat4<T> mat{};
        mat.row(3, vec4<T>{T(0.), T(0.), T(0.), T(1.)});
        mat.column(2, d);
        mat.column(1, up);
        mat.column(0, maths::cross(d, up));
        return mat;
    }

    template <typename T>
    mat4<T> look_at(const vec3<T>& look, const vec3<T>& eye, const vec3<T>& world_up=vec3<T>{T(0.), T(1.), T(0.)}) {
        auto d = normalise(look - eye);
        auto r = normalise(cross(d, world_up));
        auto u = cross(r, d);

        mat4<T> M;
        M.row(0, r);
        M.row(1, u);
        M.row(2, -d);
        M.row(3, vec3<T>{0.});
        M.column(3, vec4<T>{-dot(r, eye), -dot(u, eye), dot(d, eye), 1.});
        return M;
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
    vec3<T> operator*(const mat4<T>& lhs, const vec3<T>& v) {
        vec3<T> r;
        r[0] = lhs(0,0)*v[0] + lhs(0,1)*v[1] + lhs(0,2)*v[2];
        r[1] = lhs(1,0)*v[0] + lhs(1,1)*v[1] + lhs(1,2)*v[2];
        r[2] = lhs(2,0)*v[0] + lhs(2,1)*v[1] + lhs(2,2)*v[2];
        return r;
    }

    template <typename T>
    vec4<T> operator*(const mat4<T>& lhs, const vec4<T>& v) {
        vec4<T> r;
        r[0] = lhs(0,0)*v[0] + lhs(0,1)*v[1] + lhs(0,2)*v[2] + lhs(0,3)*v[3];
        r[1] = lhs(1,0)*v[0] + lhs(1,1)*v[1] + lhs(1,2)*v[2] + lhs(1,3)*v[3];
        r[2] = lhs(2,0)*v[0] + lhs(2,1)*v[1] + lhs(2,2)*v[2] + lhs(2,3)*v[3];
        r[3] = v[3];
        return r;
    }

    template <typename T>
    vec3<T> transform4(const mat4<T>& lhs, const vec3<T>& v) {
        vec3<T> r;
        r[0] = lhs(0,0)*v[0] + lhs(0,1)*v[1] + lhs(0,2)*v[2] + lhs(0,3);
        r[1] = lhs(1,0)*v[0] + lhs(1,1)*v[1] + lhs(1,2)*v[2] + lhs(1,3);
        r[2] = lhs(2,0)*v[0] + lhs(2,1)*v[1] + lhs(2,2)*v[2] + lhs(2,3);
        return r;
    }

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

    template <typename T>
    mat4<T> transpose(const mat4<T>& M) {
        auto R = M;
        std::swap(R(0,1), R(1,0)); std::swap(R(0,2),R(2,0)); std::swap(R(0,3),R(3,0));
        std::swap(R(1,2), R(2,1)); std::swap(R(1,3),R(3,1)); std::swap(R(2,3),R(3,2));
        return R;
    }

    template <typename T>
    mat4<T> mat4<T>::inverse(T epsilon) const {
        T a0 = arr[ 0]*arr[ 5] - arr[ 1]*arr[ 4];
        T a1 = arr[ 0]*arr[ 6] - arr[ 2]*arr[ 4];
        T a2 = arr[ 0]*arr[ 7] - arr[ 3]*arr[ 4];
        T a3 = arr[ 1]*arr[ 6] - arr[ 2]*arr[ 5];
        T a4 = arr[ 1]*arr[ 7] - arr[ 3]*arr[ 5];
        T a5 = arr[ 2]*arr[ 7] - arr[ 3]*arr[ 6];
        T b0 = arr[ 8]*arr[13] - arr[ 9]*arr[12];
        T b1 = arr[ 8]*arr[14] - arr[10]*arr[12];
        T b2 = arr[ 8]*arr[15] - arr[11]*arr[12];
        T b3 = arr[ 9]*arr[14] - arr[10]*arr[13];
        T b4 = arr[ 9]*arr[15] - arr[11]*arr[13];
        T b5 = arr[10]*arr[15] - arr[11]*arr[14];

        T det = a0*b5 - a1*b4 + a2*b3 + a3*b2 - a4*b1 + a5*b0;
        if(std::abs(det) <= epsilon) return mat4((T)0);

        mat4 inv;
        inv[ 0] = + arr[ 5]*b5 - arr[ 6]*b4 + arr[ 7]*b3;
        inv[ 4] = - arr[ 4]*b5 + arr[ 6]*b2 - arr[ 7]*b1;
        inv[ 8] = + arr[ 4]*b4 - arr[ 5]*b2 + arr[ 7]*b0;
        inv[12] = - arr[ 4]*b3 + arr[ 5]*b1 - arr[ 6]*b0;
        inv[ 1] = - arr[ 1]*b5 + arr[ 2]*b4 - arr[ 3]*b3;
        inv[ 5] = + arr[ 0]*b5 - arr[ 2]*b2 + arr[ 3]*b1;
        inv[ 9] = - arr[ 0]*b4 + arr[ 1]*b2 - arr[ 3]*b0;
        inv[13] = + arr[ 0]*b3 - arr[ 1]*b1 + arr[ 2]*b0;
        inv[ 2] = + arr[13]*a5 - arr[14]*a4 + arr[15]*a3;
        inv[ 6] = - arr[12]*a5 + arr[14]*a2 - arr[15]*a1;
        inv[10] = + arr[12]*a4 - arr[13]*a2 + arr[15]*a0;
        inv[14] = - arr[12]*a3 + arr[13]*a1 - arr[14]*a0;
        inv[ 3] = - arr[ 9]*a5 + arr[10]*a4 - arr[11]*a3;
        inv[ 7] = + arr[ 8]*a5 - arr[10]*a2 + arr[11]*a1;
        inv[11] = - arr[ 8]*a4 + arr[ 9]*a2 - arr[11]*a0;
        inv[15] = + arr[ 8]*a3 - arr[ 9]*a1 + arr[10]*a0;

        T invDet = (T)1/det;
        inv[ 0] *= invDet;
        inv[ 1] *= invDet;
        inv[ 2] *= invDet;
        inv[ 3] *= invDet;
        inv[ 4] *= invDet;
        inv[ 5] *= invDet;
        inv[ 6] *= invDet;
        inv[ 7] *= invDet;
        inv[ 8] *= invDet;
        inv[ 9] *= invDet;
        inv[10] *= invDet;
        inv[11] *= invDet;
        inv[12] *= invDet;
        inv[13] *= invDet;
        inv[14] *= invDet;
        inv[15] *= invDet;

        return inv;
    }

    using mat4b = mat4<uint8_t>;
    using mat4s = mat4<int16_t>;
    using mat4i = mat4<int32_t>;
    using mat4f = mat4<float>;
    using mat4d = mat4<double>;
}}
#endif //ZAP_MAT4_HPP

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
    struct mat2 {
        using type = T;

        static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value, "mat2<T>: T must be an algebraic type");

        using row_t = vec2<T>;
        using col_t = vec2<T>;

        constexpr static size_t size() { return 4; }
        constexpr static size_t memsize() { return sizeof(mat2<T>); }
        constexpr static size_t cols() { return 2; }
        constexpr static size_t rows() { return 2; }
        constexpr static size_t idx(size_t row, size_t col) { return col*rows() + row; }

        constexpr mat2() { }
        constexpr explicit mat2(T init) : m00(init), m10(init), m01(init), m11(init) { }
        constexpr mat2(const static_list<T, 4>& lst) : m00(lst[0]), m10(lst[2]), m01(lst[1]), m11(lst[3]) { }
        constexpr mat2(T m00, T m01, T m10, T m11) : m00(m00), m10(m10), m01(m01), m11(m11) { }
        constexpr explicit mat2(T m00, T m11) : m00(m00), m10(T(0)), m01(T(0)), m11(m11) { }
        constexpr mat2(const col_t& col0, const col_t& col1) : m00(col0[0]), m10(col0[1]), m01(col1[0]), m11(col1[1]) {}

        constexpr static mat2 make_row(const row_t& row0, const row_t& row1) {
            return mat2(row0[0], row0[1], row1[0], row1[1]);
        }
        constexpr static mat2 make_col(const col_t& col0, const col_t& col1) {
            return mat2(col0[0], col1[0], col0[1], col1[1]);
        }
        static inline mat2 make_polar(T theta) {
            static_assert(std::is_floating_point<T>::value, ZERR_TYPE_FLOATING);
            return mat2(std::cos(theta), std::sin(theta));
        }

        inline T* begin() { return arr; }
        inline T* end() { return arr + size(); }
        inline const T* begin() const { return arr; }
        inline const T* end() const { return arr + size(); }
        inline const T* data() const { return arr; }
        inline T* data() { return arr; }

        const T& operator()(size_t row, size_t col) const {
            assert(row < rows() && col < cols() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx(row,col)];
        }

        T& operator()(size_t row, size_t col) {
            assert(row < rows() && col < cols() && ZERR_IDX_OUT_OF_RANGE);
            return arr[idx(row,col)];
        }

        union {
            struct {
                T m00, m10,     // Column 0
                  m01, m11;     // Column 1
            };
            T arr[size()];
        };
	}; // __attribute__((aligned(16)));
}}

#endif //ZAP_MAT2_HPP

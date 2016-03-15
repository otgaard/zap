//
// Created by Darren Otgaar on 2016/03/08.
//

#ifndef ZAP_MAT2_HPP
#define ZAP_MAT2_HPP

#include "maths.hpp"

/* Note:
 * Matrices are represented in standard mathematical notation:
 * The matrix M as R rows and C columns.
 * An element is represented by [r,c].
 * Matrices are stored in column major order.
 *
 * Note that memory in C++ is allocated in row major form, thus the initialiser:
 *
 *  {
 *      { 1, 2 }
 *      { 3, 4 }
 *  }
 *
 * Will result in
 */

namespace zap { namespace maths {
    template <typename T>
    class mat2 {
    public:
        constexpr static inline size_t size() { return 4; }
        constexpr static inline size_t memsize() { return sizeof(mat2<T>); }
        constexpr static inline size_t cols() { return 2; }
        constexpr static inline size_t rows() { return 2; }
        constexpr static inline size_t idx(size_t row, size_t col) { return col*rows() + row; }

        constexpr inline mat2() { }
        constexpr inline mat2(const static_list<T, 4>& lst) : m00(lst[0]), m10(lst[2]), m01(lst[1]), m11(lst[3]) { }
        constexpr inline mat2(const static_list<T,2>& row0, const static_list<T,2>& row1) :
                m00(row0[0]), m10(row1[0]), m01(row0[1]), m11(row1[1]) { }
        constexpr inline mat2(T m00, T m01, T m10, T m11) : m00(m00), m10(m10), m01(m01), m11(m11) { }

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
    } __attribute__((aligned(16)));
}}

#endif //ZAP_MAT2_HPP

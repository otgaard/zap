//
// Created by Darren Otgaar on 2016/07/03.
//

#ifndef ZAP_MATRIX_HPP
#define ZAP_MATRIX_HPP

#include "maths.hpp"
#include "vector.hpp"

namespace zap { namespace maths {
    template <typename T, size_t Rows, size_t Cols>
    struct matrix {
        using type = T;
        constexpr static size_t rows = Rows;
        constexpr static size_t cols = Cols;
        constexpr static size_t size = rows*cols;

        using row_vector = vector<T, rows>;
        using col_vector = vector<T, cols>;

        constexpr static size_t idx(size_t row, size_t col) { return col*rows + row; }
        constexpr static size_t ridx(size_t row, size_t col) { return row*cols + col; }

        matrix() = default;
        explicit matrix(const T& init) { for(int i = 0; i != size; ++i) arr[i] = init; }
        matrix(const static_list<T, size>& lst) {
            for(int col = 0; col != cols; ++col)
                for(int row = 0; row != rows; ++row)
                    arr[idx(row,col)] = lst[ridx(row,col)];
        }
        matrix(const matrix& rhs) { for(int i = 0; i != size; ++i) arr[i] = rhs[i]; }

        matrix operator=(const matrix& rhs) { if(this != &rhs) for(int i = 0; i != size; ++i) arr[i] = rhs[i]; return *this; }

        const T& operator[](size_t idx) const { checkidx(idx, size); return arr[idx]; };
        T& operator[](size_t idx) { checkidx(idx, size); return arr[idx]; }
        const T& operator()(size_t row, size_t col) const { checkidx(idx(row,col), size); return arr[idx(row,col)]; }
        T& operator()(size_t row, size_t col) { checkidx(idx(row,col), size); return arr[idx(row,col)]; }
        const T* data() const { return &arr[0]; }
        T* data() { return &arr[0]; }

        matrix& operator+=(const matrix& rhs) { for(int i = 0; i != size; ++i) arr[i] += rhs[i]; return *this; }
        matrix& operator-=(const matrix& rhs) { for(int i = 0; i != size; ++i) arr[i] -= rhs[i]; return *this; }
        template <typename S> matrix& operator*=(const S& scalar) { for(int i = 0; i != size; ++i) arr[i] *= scalar; return *this; }
        template <typename S> matrix& operator/=(const S& scalar) { const auto inv_s = S(1)/scalar; return operator*=(inv_s); }

        T arr[size];
    };

    template <typename LhsT, typename RhsT, size_t R, size_t CR, size_t C>
    matrix<decltype(LhsT() * RhsT()), R, C> operator*(const matrix<LhsT, R, CR>& lhs, const matrix<RhsT, CR, C>& rhs) {
        matrix<decltype(LhsT() * RhsT()), R, C> result(decltype(LhsT()*RhsT())(0.0));
        for(size_t col = 0; col != matrix<RhsT, CR, C>::cols; ++col) {
            for(size_t row = 0; row != matrix<LhsT, R, CR>::rows; ++row) {
                for(size_t i = 0; i != matrix<RhsT, CR, C>::rows; ++i) {
                    result(row,col) += lhs(row,i) * rhs(i,col);
                }
            }
        }
        return result;
    }

    template <typename LhsT, typename RhsT, size_t R, size_t C>
    vector<decltype(LhsT() * RhsT()), C> operator*(const matrix<LhsT, R, C>& lhs, const vector<RhsT, C>& rhs) {
        vector<decltype(LhsT() * RhsT()), C> result(decltype(LhsT()*RhsT())(0.0));
        for(size_t col = 0; col != matrix<LhsT, R, C>::cols; ++col) {
            for(size_t row = 0; row != matrix<LhsT, R, C>::rows; ++row) {
                result[row] += lhs(row,col) * rhs[row];
            }
        }
        return result;
    }
}}

#endif //ZAP_MATRIX_HPP

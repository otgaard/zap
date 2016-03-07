/* Created by Darren Otgaar on 2016/03/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_MATHS_HPP
#define ZAP_MATHS_HPP

#include <cmath>
#include <array>
#include <cassert>
#include <numeric>

#define ZERR_IDX_OUT_OF_RANGE "Index out of range"
#define ZERR_ARG_COUNT_INVALID "Incorrect number of values provided"

// Type and size safe static initialiser
template <typename T, size_t N>
struct static_list : std::array<T, N> {
    template <typename... Args>
    constexpr static_list(Args... args) : std::array<T, N>( {{ args... }} ) {
        static_assert(sizeof...(args) == N, ZERR_ARG_COUNT_INVALID);
    }
};

#endif //ZAP_MATHS_HPP

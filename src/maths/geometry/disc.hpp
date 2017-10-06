//
// Created by Darren Otgaar on 2016/06/04.
//

#ifndef ZAP_DISC_HPP
#define ZAP_DISC_HPP

#include "maths/maths.hpp"

namespace zap { namespace maths { namespace geometry {

template <typename T>
struct disc {
    using vector = vec2<T>;
    using real = T;

    disc() = default;
    disc(const disc& rhs) = default;
    disc(const vec2<T>& P, T radius) : P(P), r(radius) { }

    disc& operator=(const disc& rhs) = default;

    vector P;
    real r;
};

template <typename T>
T distance(const vec2<T>& P, const disc<T>& D) {
    return std::sqrt((D.P - P).length_sqr()) - D.r;
}

template <typename T>
T distance_sqr(const disc<T>& A, const disc<T>& B) {
    return (A.P - B.P).length_sqr() - (A.r*A.r) - (B.r*B.r);
}

template <typename T>
T distance(const disc<T>& A, const disc<T>& B) {
    return std::sqrt(distance_sqr(A, B));
}

}}}

#endif //ZAP_DISC_HPP

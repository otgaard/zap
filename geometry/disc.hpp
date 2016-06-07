//
// Created by Darren Otgaar on 2016/06/04.
//

#ifndef ZAP_DISC_HPP
#define ZAP_DISC_HPP

#include "maths/maths.hpp"

namespace zap { namespace maths {

template <typename T>
struct disc {
    using vector = vec2<T>;
    using real = T;

    disc() { }
    disc(const vec2<T>& P, T radius) : P(P), r(radius) { }

    vector P;
    real r;
};

template <typename T>
T distance(const vec2<T>& P, const disc<T>& D) {
    return std::sqrt((D.P - P).length_sq()) - D.r;
}

template <typename T>
T distance_sq(const disc<T>& A, const disc<T>& B) {
    return (A.P - B.P).length_sq() - (A.r*A.r) - (B.r*B.r);
}

template <typename T>
T distance(const disc<T>& A, const disc<T>& B) {
    return std::sqrt(distance_sq(A, B));
}

}}

#endif //ZAP_DISC_HPP

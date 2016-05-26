//
// Created by Darren Otgaar on 2016/05/22.
//

#ifndef ZAP_RAY_HPP
#define ZAP_RAY_HPP

#include "maths/maths.hpp"

namespace zap { namespace maths {

template <typename VecType>
struct ray {
    using vector = VecType;

    ray() { }
    ray(const vector& P, const vector& d) : P(P), d(d) { }

    vector P;
    vector d;
};

}}

#endif //ZAP_RAY_HPP

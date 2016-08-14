//
// Created by Darren Otgaar on 2016/05/22.
//

#ifndef ZAP_RAY_HPP
#define ZAP_RAY_HPP

#include "maths/maths.hpp"

namespace zap { namespace maths { namespace geometry {

template <typename VecType>
struct ray {
    using type = typename VecType::type;
    using vector = VecType;

    ray() = default;
    ray(const vector& O, const vector& d) : O(O), d(d) { }

    inline const vector& origin() const { return O; }
    inline const vector& dir() const { return d; }

    inline vector position(const type& t) const { return O + t*d; }

    vector O;
    vector d;
};

}}}

#endif //ZAP_RAY_HPP

//
// Created by Darren Otgaar on 2016/05/22.
//

#ifndef ZAP_RAY_HPP
#define ZAP_RAY_HPP

#include <maths/algebra.hpp>

namespace zap { namespace maths { namespace geometry {

template <typename VecType>
struct ray {
    using type = typename VecType::type;
    using vector = VecType;

    ray() = default;
    ray(const ray& rhs) = default;
    ray(const vector& O, const vector& d) : O(O), d(d) { }

    ray& operator=(const ray& rhs) = default;

    inline const vector& origin() const { return O; }
    inline const vector& dir() const { return d; }

    inline vector position(const type& t) const { return O + t*d; }

    vector O;
    vector d;
};

using ray2i = ray<vec2i>;
using ray2f = ray<vec2f>;
using ray2d = ray<vec2d>;
using ray3i = ray<vec3i>;
using ray3f = ray<vec3f>;
using ray3d = ray<vec3d>;

}}}

#endif //ZAP_RAY_HPP

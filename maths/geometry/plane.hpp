//
// Created by Darren Otgaar on 2016/08/07.
//

#ifndef ZAP_PLANE_HPP
#define ZAP_PLANE_HPP

#include "maths/maths.hpp"
#include "ray.hpp"

namespace zap { namespace maths { namespace geometry {

template <typename T>
struct plane {
    using vector = vec3<T>;
    using real = T;

    plane() { }
    plane(const vec3<T>& O, const vec3<T>& n) : O(O), n(n) { }

    vector O;
    vector n;
};

template <typename T>
T intersection(const plane<T>& P, const ray<vec3<T>>& R) {
    return dot((R.P - P.O), P.n)/dot(R.d, P.n);
}

}}}

#endif //ZAP_PLANE_HPP

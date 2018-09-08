//
// Created by Darren Otgaar on 2016/08/07.
//

#ifndef ZAP_PLANE_HPP
#define ZAP_PLANE_HPP

#include <maths/maths.hpp>
#include <maths/geometry/ray.hpp>

namespace zap { namespace maths { namespace geometry {

template <typename T>
struct plane {
    using vector = vec3<T>;
    using real = T;

    plane() = default;
    plane(real a, real b, real c, real d) : a(a), b(b), c(c), d(d) { }
    plane(const plane& rhs) = default;
    plane(const vector& O, const vector& n) { *this = make_plane(O, n); }

    static plane make_plane(const vector& O, const vector& n) {
        plane P;
        const auto N = !n.is_unit() ? maths::normalise(n) : n;
        P.a = N.x; P.b = N.y; P.c = N.z;
        P.d = -dot(O, N);
        return P;
    }

    static plane make_plane(const vector& P0, const vector& P1, const vector& P2) {
        const auto N = maths::normalise(cross(P1 - P0, P2 - P0));
        return make_plane(P0, N);
    }

    plane& operator=(const plane& rhs) = default;

    real distance(const vector& P0) const {
        return a * P0.x + b * P0.y + c * P0.z + d;
    }

    plane& normalise() {
        const auto inv_d = T(1) / std::sqrt<T>(a*a + b*b + c*c);
        a *= inv_d; b *= inv_d; c *= inv_d; d *= inv_d;
        return *this;
    }

    const vector origin() const {
        const vector O(0, 0, 0);
        return O - distance(O)*normal();
    }

    const vector normal() const { return vector(a, b, c); }

    real a, b, c, d;
};

using plane3f = plane<float>;
using plane3d = plane<double>;

template <typename T>
T distance(const plane<T>& P, const typename plane<T>::vector& P0) {
    return P.distance(P0);
}

template <typename T>
T intersection(const plane<T>& P, const ray<typename plane<T>::vector>& R) {
    return dot((P.origin() - R.O), P.normal())/dot(R.d, P.normal());
}

}}}

#endif //ZAP_PLANE_HPP

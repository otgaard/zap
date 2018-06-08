//
// Created by Darren Otgaar on 2016/08/07.
//

#ifndef ZAP_SPHERE_HPP
#define ZAP_SPHERE_HPP

#include <limits>
#include <maths/geometry/ray.hpp>

namespace zap { namespace maths { namespace geometry {

template <typename T>
struct sphere {
    using type = T;
    using vector_t = vec3<type>;
    using affine_t = mat4<type>;

    sphere() = default;
    sphere(const sphere& rhs) = default;
    sphere(const vector_t& centre, const type& r) : centre(centre), radius(r) { }

    sphere& operator=(const sphere& rhs) = default;

    sphere& translate(const vector_t& trans) {
        centre += trans;
        return *this;
    }

    sphere& transform(const affine_t& trans) {
        centre = trans.transform(centre);
        return *this;
    }

    vector_t centre;
    type radius;
};

using spheref = sphere<float>;
using sphered = sphere<double>;

template <typename T>
int intersection(const sphere<T>& S, const ray<typename sphere<T>::vector_t>& R, T t[2], const T& epsilon=std::numeric_limits<T>::epsilon()) {
    using vector = typename sphere<T>::vector_t;
    vector v = R.O - S.centre;
    T a = R.d.length_sqr(), b = T(2) * dot(R.d, v), c = v.length_sqr() - S.radius*S.radius;
    T discrim = b*b - 4*a*c;
    if(discrim > 0) {           // Two Roots
        T a2 = 2*a, rt = std::sqrt(discrim);
        t[0] = (-b + rt) / a2; // Larger Root First
        t[1] = (-b - rt) / a2;
        return int(t[0] >= epsilon) + int(t[1] >= epsilon);
    } else if(discrim == 0) {   // One root
        t[0] = -b / (2*a);
        return int(t[0] >= epsilon);
    }

    return 0;
}

}}}

#endif //ZAP_SPHERE_HPP

//
// Created by Darren Otgaar on 2016/08/07.
//

#ifndef ZAP_SPHERE_HPP
#define ZAP_SPHERE_HPP

namespace zap { namespace maths { namespace geometry {

template <typename T>
struct sphere {
    using type = T;
    using vector = vec3<type>;

    sphere() = default;
    sphere(const sphere& rhs) = default;
    sphere(const vector& C, const type& r) : C(C), r(r) { }

    sphere& operator=(const sphere& rhs) = default;

    inline const vector& centre() const { return C; }
    inline const type& radius() const { return r; }

    vector C;
    type r;
};

template <typename T>
int intersection(const sphere<T>& S, const ray<typename sphere<T>::vector>& R, T t[2]) {
    using vector = typename sphere<T>::vector;
    vector v = R.O - S.C;
    T a = R.d.length_sqr(), b = T(2) * dot(R.d, v), c = v.length_sqr() - S.r*S.r;
    T discrim = b*b - 4*a*c;
    if(discrim > 0) {           // Two Roots
        T a2 = 2*a, rt = std::sqrt(discrim);
        t[0] = (-b + rt) / a2; // Larger Root First
        t[1] = (-b - rt) / a2;
        return int(t[0] >= 0) + int(t[1] >= 0); // Epsilon?
    } else if(discrim == 0) {   // One root
        t[0] = -b / (2*a);
        return int(t[0] >= 0);
    }

    return 0;
}

}}}

#endif //ZAP_SPHERE_HPP

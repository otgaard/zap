//
// Created by Darren Otgaar on 2016/06/04.
//

#ifndef ZAP_DISC_HPP
#define ZAP_DISC_HPP

#include <maths/maths.hpp>

namespace zap { namespace maths { namespace geometry {

template <typename T>
struct disc {
    using type = T;
    using vector_t = vec2<T>;
    using affine_t = mat3<T>;

    disc() = default;
    disc(const disc& rhs) = default;
    disc(const vec2<T>& centre, T radius) : centre(centre), radius(radius) { }

    disc& translate(const vector_t& trans) {
        centre += trans;
        return *this;
    }

    disc& transform(const affine_t& trans) {
        centre += trans.col(2);
        return *this;
    }

    disc& operator=(const disc& rhs) = default;

    vector_t centre;
    type radius;
};

template <typename T>
T distance(const vec2<T>& P, const disc<T>& D) {
    return std::sqrt((D.centre - P).length_sqr()) - D.radius;
}

template <typename T>
T distance_sqr(const disc<T>& A, const disc<T>& B) {
    return (A.centre - B.centre).length_sqr() - A.radius*A.radius - B.radius*B.radius;
}

template <typename T>
T distance(const disc<T>& A, const disc<T>& B) {
    return std::sqrt(distance_sqr(A, B));
}

template <typename T>
bool intersection(const disc<T>& D, const vec2<T>& P) {
    return (P - D.centre).length_sqr() < D.radius*D.radius;
}

template <typename T>
bool intersection(const disc<T>& A, const disc<T>& B) {
    return (A.centre - B.centre).length_sqr() < A.radius*A.radius + B.radius*B.radius;
}

using disc2i = disc<int>;
using disc2f = disc<float>;
using disc2d = disc<double>;

}}}

#endif //ZAP_DISC_HPP

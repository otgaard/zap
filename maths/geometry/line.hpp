//
// Created by Darren Otgaar on 2016/10/07.
//

#ifndef ZAP_LINE_HPP
#define ZAP_LINE_HPP

#include "maths/algebra.hpp"

namespace zap { namespace maths { namespace geometry {

template <typename VecType>
struct line {
    using type = typename VecType::type;
    using vector = VecType;

    line() = default;
    line(const line& rhs) = default;
    line(const vector& O, const vector& d) : O(O), d(d) { }

    line& operator=(const line& rhs) = default;

    inline const vector& origin() const { return O; }
    inline const vector& dir() const { return d; }

    inline vector position(const type& t) const { return O + t*d; }

    vector O;
    vector d;
};

using line2i = line<vec2i>;
using line2f = line<vec2f>;
using line2d = line<vec2d>;
using line3i = line<vec3i>;
using line3f = line<vec3f>;
using line3d = line<vec3d>;

}}}

#endif //ZAP_LINE_HPP

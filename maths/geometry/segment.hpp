//
// Created by Darren Otgaar on 2016/10/07.
//

#ifndef ZAP_SEGMENT_HPP
#define ZAP_SEGMENT_HPP

#include "maths/algebra.hpp"

namespace zap { namespace maths { namespace geometry {

template <typename VecType>
struct segment {
    using type = typename VecType::type;
    using vector = VecType;

    segment() = default;
    segment(const vector& P0, const vector& P1) : P0(P0), P1(P1) { }

    inline const vector& origin() const { return P0; }
    inline const vector dir() const { return normalise(P1 - P0); }

    inline vector position(const type& t) const { return lerp(t, P0, P1); }

    vector P0;
    vector P1;
};

using segment2i = segment<vec2i>;
using segment2f = segment<vec2f>;
using segment2d = segment<vec2d>;
using segment3i = segment<vec3i>;
using segment3f = segment<vec3f>;
using segment3d = segment<vec3d>;

}}}

#endif //ZAP_SEGMENT_HPP

//
// Created by Darren Otgaar on 2016/09/17.
//

#ifndef ZAP_HULL_HPP
#define ZAP_HULL_HPP

#include <vector>
#include <maths/vec2.hpp>

namespace zap { namespace maths { namespace geometry {

template <typename VecT>
struct hull {
    using type = typename VecT::type;
    using vector = VecT;

    hull() = default;
    hull(const hull& rhs) = default;
    explicit hull(const std::vector<vector>& vertices) : vertices(vertices) { }

    hull& operator=(const hull& rhs) = default;

    std::vector<vector> vertices;
};

// 2D SAT for planar hulls - (requires counter-clockwise ordering of vertices)
bool intersection(const std::vector<vec2f>& hullA, const std::vector<vec2f>& hullB) {
    auto side = [](const std::vector<vec2f>& hull, const vec2f& d, const vec2f& P) {
        int pos = 0, neg = 0;
        for(int i = 0; i != hull.size(); ++i) {
            auto t = dot(d, hull[i] - P);
            if(t > 0)      ++pos;
            else if(t < 0) ++neg;
            if(pos && neg) return 0;
        }
        return pos ? 1 : -1;
    };

    for(size_t v0 = 0, v1 = hullA.size()-1; v0 != hullA.size(); v1 = v0, ++v0) {
        auto d = perp(hullA[v1] - hullA[v0]);
        if(side(hullB, d, hullA[v0]) > 0) return false;
    }

    for(size_t v0 = 0, v1 = hullB.size()-1; v0 != hullB.size(); v1 = v0, ++v0) {
        auto d = perp(hullB[v1] - hullB[v0]);
        if(side(hullA, d, hullB[v0]) > 0) return false;
    }

    return true;
}

}}}

#endif //ZAP_HULL_HPP

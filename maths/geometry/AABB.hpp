//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_AABB_HPP
#define ZAP_AABB_HPP

#include <maths/algebra.hpp>

namespace zap { namespace maths { namespace geometry {
    template <typename T, template <typename> class VecT> struct AABB;

    template <typename T>
    struct AABB<T, vec2> {
        using type = T;
        using vector = vec2<T>;

        AABB() = default;
        AABB(const vector& hextent) : centre(type(0), type(0)), hextent(hextent) { }
        AABB(const vector& centre, const vector& hextent) : centre(centre), hextent(hextent) { }
        AABB(const vector& startp, type width, type height) : centre(startp.x + width/2, startp.y + height/2),
                                                              hextent(width/2, height/2) { }
        AABB(const AABB& rhs) = default;

        AABB& operator=(const AABB& rhs) = default;

        inline type width() const { return 2*hextent.x; }
        inline type height() const { return 2*hextent.y; }
        inline type left() const { return centre.x - hextent.x; }
        inline type right() const { return centre.x + hextent.x; }
        inline type bottom() const { return centre.y - hextent.y; }
        inline type top() const { return centre.y + hextent.y; }
        inline vector min() const { return centre - hextent; }
        inline vector max() const { return centre + hextent; }
        inline type radius() const { return type(.5)*normalise(max() - min()); }

        vector centre;
        vector hextent;
    };

    using AABB2i = AABB<int, vec2>;
    using AABB2f = AABB<float, vec2>;
    using AABB2d = AABB<double, vec2>;

    template <typename T>
    struct AABB<T, vec3> {
        using type = T;
        using vector = vec3<T>;

        AABB() = default;
        AABB(const vector& hextent) : centre(type(0), type(0)), hextent(hextent) { }
        AABB(const vector& centre, const vector& hextent) : centre(centre), hextent(hextent) { }
        AABB(const vector& startp, type width, type height, type depth) :
                centre(startp.x + width/2, startp.y + height/2, startp.z + depth/2),
                hextent(width/2, height/2, depth/2) { }
        AABB(const AABB& rhs) = default;

        AABB& operator=(const AABB& rhs) = default;

        inline type width() const { return 2*hextent.x; }
        inline type height() const { return 2*hextent.y; }
        inline type depth() const { return 2*hextent.z; }
        inline type left() const { return centre.x - hextent.x; }
        inline type right() const { return centre.x + hextent.x; }
        inline type bottom() const { return centre.y - hextent.y; }
        inline type top() const { return centre.y + hextent.y; }
        inline type back() const { return centre.z - hextent.z; }
        inline type front() const { return centre.z + hextent.z; }
        inline vector min() const { return centre - hextent; }
        inline vector max() const { return centre + hextent; }
        inline type radius() const { return type(.5)*normalise(max() - min()); }

        vector centre;
        vector hextent;
    };

    using AABB3i = AABB<int, vec3>;
    using AABB3f = AABB<float, vec3>;
    using AABB3d = AABB<double, vec3>;
}}}

#endif //ZAP_AABB_HPP

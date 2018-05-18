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
        using vector_t = vec2<T>;
        using affine_t = mat3<T>;

        AABB() = default;
        AABB(const vector_t& hextent) : centre(type(0), type(0)), hextent(hextent) { }
        AABB(const vector_t& centre, const vector_t& hextent) : centre(centre), hextent(hextent) { }
        AABB(const vector_t& startp, type width, type height) : centre(startp.x + width/2, startp.y + height/2),
                                                              hextent(width/2, height/2) { }
        AABB(const AABB& rhs) = default;

        AABB& operator=(const AABB& rhs) = default;

        AABB& translate(const vector_t& t) {
            centre += t;
            return *this;
        }

        // Note: Repeatedly rotating an AABB will increase its size to infinity, rather use OBB
        AABB& transform(const affine_t& T) {
            vector_t arr[4] = {
                centre + -hextent,
                centre + vector_t{+1, -1} * hextent,
                centre + vector_t{-1, +1} * hextent,
                centre + hextent
            };

            std::transform(arr, arr+4, arr, [&T](vector_t& P) { return T.transform(P); });

            type x_min = std::numeric_limits<type>::max(), x_max = -x_min;
            type y_min = x_min, y_max = x_max;

            for(auto& p : arr) {
                if(p.x < x_min) x_min = p.x;
                if(p.x > x_max) x_max = p.x;
                if(p.y < y_min) y_min = p.y;
                if(p.y > y_max) y_max = p.y;
            }

            hextent.set((x_max - x_min)/2, (y_max - y_min)/2);
            centre.set(x_min + hextent.x, y_min + hextent.y);
            return *this;
        }

        inline type width() const { return 2*hextent.x; }
        inline type height() const { return 2*hextent.y; }
        inline type left() const { return centre.x - hextent.x; }
        inline type right() const { return centre.x + hextent.x; }
        inline type bottom() const { return centre.y - hextent.y; }
        inline type top() const { return centre.y + hextent.y; }
        inline vector_t min() const { return centre - hextent; }
        inline vector_t max() const { return centre + hextent; }
        inline type radius() const { return type(.5)*normalise(max() - min()); }

        vector_t centre;
        vector_t hextent;
    };

    using AABB2i = AABB<int, vec2>;
    using AABB2f = AABB<float, vec2>;
    using AABB2d = AABB<double, vec2>;

    template <typename T>
    struct AABB<T, vec3> {
        using type = T;
        using vector_t = vec3<T>;
        using affine_t = mat4<T>;

        AABB() = default;
        AABB(const vector_t& hextent) : centre(type(0), type(0)), hextent(hextent) { }
        AABB(const vector_t& centre, const vector_t& hextent) : centre(centre), hextent(hextent) { }
        AABB(const vector_t& startp, type width, type height, type depth) :
                centre(startp.x + width/2, startp.y + height/2, startp.z + depth/2),
                hextent(width/2, height/2, depth/2) { }
        AABB(const AABB& rhs) = default;

        AABB& operator=(const AABB& rhs) = default;

        AABB& translate(const vector_t& t) {
            centre += t;
            return *this;
        }

        AABB& transform(const affine_t& T) {
            centre = T.transform(centre);
            auto new_extent = T * hextent;
            vector_t arr[8] = {
                    -new_extent,
                    vector_t{+1, -1, -1} * new_extent,
                    vector_t{+1, +1, -1} * new_extent,
                    vector_t{-1, +1, -1} * new_extent,
                    new_extent,
                    vector_t{-1, -1, +1} * new_extent,
                    vector_t{+1, -1, +1} * new_extent,
                    vector_t{-1, +1, +1} * new_extent
            };

            type x_min = std::numeric_limits<type>::max(), x_max = -x_min;
            type y_min = x_min, y_max = x_max;
            type z_min = x_min, z_max = x_max;

            for(auto& p : arr) {
                if(p.x < x_min) x_min = p.x;
                if(p.x > x_max) x_max = p.x;
                if(p.y < y_min) y_min = p.y;
                if(p.y > y_max) y_max = p.y;
                if(p.z < z_min) z_min = p.z;
                if(p.z > z_max) z_max = p.z;
            }

            hextent.set((x_max - x_min)/2, (y_max - y_min)/2, (z_max - z_min)/2);
            centre.set(x_min + hextent.x, y_min + hextent.y, z_min + hextent.z);
            return *this;
        }


        inline type width() const { return 2*hextent.x; }
        inline type height() const { return 2*hextent.y; }
        inline type depth() const { return 2*hextent.z; }
        inline type left() const { return centre.x - hextent.x; }
        inline type right() const { return centre.x + hextent.x; }
        inline type bottom() const { return centre.y - hextent.y; }
        inline type top() const { return centre.y + hextent.y; }
        inline type back() const { return centre.z - hextent.z; }
        inline type front() const { return centre.z + hextent.z; }
        inline vector_t min() const { return centre - hextent; }
        inline vector_t max() const { return centre + hextent; }
        inline type radius() const { return type(.5)*normalise(max() - min()); }

        vector_t centre;
        vector_t hextent;
    };

    using AABB3i = AABB<int, vec3>;
    using AABB3f = AABB<float, vec3>;
    using AABB3d = AABB<double, vec3>;
}}}

#endif //ZAP_AABB_HPP

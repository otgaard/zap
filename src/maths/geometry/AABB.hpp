//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_AABB_HPP
#define ZAP_AABB_HPP

#include <maths/algebra.hpp>
#include <maths/geometry/ray.hpp>

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

    AABB& translate(const vector_t& trans) {
        centre += trans;
        return *this;
    }

    // Note: Repeatedly rotating an AABB will increase its size to infinity, rather use OBB
    AABB& transform(const affine_t& trans) {
        vector_t arr[4] = {
            centre + -hextent,
            centre + vector_t{+1, -1} * hextent,
            centre + vector_t{-1, +1} * hextent,
            centre + hextent
        };

        for(auto& P : arr) P = trans.transform(P);

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

    type width() const { return 2*hextent.x; }
    type height() const { return 2*hextent.y; }
    type left() const { return centre.x - hextent.x; }
    type right() const { return centre.x + hextent.x; }
    type bottom() const { return centre.y - hextent.y; }
    type top() const { return centre.y + hextent.y; }
    vector_t min() const { return centre - hextent; }
    vector_t max() const { return centre + hextent; }
    type radius() const { return normalise(max() - min())/2; }

    vector_t left_bottom() const { return vector_t{ left(), bottom() };}
    vector_t right_bottom() const { return vector_t{ right(), bottom() };}
    vector_t left_top() const { return vector_t{ left(), top() };}
    vector_t right_top() const { return vector_t{ right(), top() };}

    vector_t centre;
    vector_t hextent;
};

template <typename T>
bool intersection(const AABB<T, vec2>& A, const vec2<T>& P) {
    return A.left() <= P.x && P.x <= A.right() &&
           A.bottom() <= P.y && P.y <= A.top();
}

inline bool intersection(const AABB<int, vec2>& A, const AABB<int, vec2>& B) {
    int r;
    r = A.hextent[0] + B.hextent[0]; if(A.centre[0] - B.centre[0] + r > r + r) return false;
    r = A.hextent[1] + B.hextent[1]; if(A.centre[1] - B.centre[1] + r > r + r) return false;
    return true;
}

template <typename T>
bool intersection(const AABB<T, vec2>& A, const AABB<T, vec2>& B) {
    if(std::abs(A.centre[0] - B.centre[0]) > (A.hextent[0] + B.hextent[0])) return false;
    if(std::abs(A.centre[1] - B.centre[1]) > (A.hextent[1] + B.hextent[1])) return false;
    return true;
}

template <typename T>
bool intersection(const AABB<T, vec2>& A, const geometry::ray<vec2<T>>& r) {
    return false;
}

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

    AABB& translate(const vector_t& trans) {
        centre += trans;
        return *this;
    }

    AABB& transform(const affine_t& trans) {
        vector_t arr[8] = {
            centre + -hextent,
            centre + vector_t{+1, -1, -1} * hextent,
            centre + vector_t{+1, +1, -1} * hextent,
            centre + vector_t{-1, +1, -1} * hextent,
            centre + hextent,
            centre + vector_t{-1, -1, +1} * hextent,
            centre + vector_t{+1, -1, +1} * hextent,
            centre + vector_t{-1, +1, +1} * hextent
        };

        for(auto& P : arr) P = trans.transform(P);

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

    type width() const { return 2*hextent.x; }
    type height() const { return 2*hextent.y; }
    type depth() const { return 2*hextent.z; }
    type left() const { return centre.x - hextent.x; }
    type right() const { return centre.x + hextent.x; }
    type bottom() const { return centre.y - hextent.y; }
    type top() const { return centre.y + hextent.y; }
    type back() const { return centre.z - hextent.z; }
    type front() const { return centre.z + hextent.z; }
    vector_t min() const { return centre - hextent; }
    vector_t max() const { return centre + hextent; }
    type radius() const { return normalise(max() - min())/2; }

    vector_t left_bottom_back() const { return vector_t{ left(), bottom(), back() }; }
    vector_t right_bottom_back() const { return vector_t{ right(), bottom(), back() }; }
    vector_t left_top_back() const { return vector_t{ left(), top(), back() }; }
    vector_t right_top_back() const { return vector_t{ right(), top(), back() }; }
    vector_t left_bottom_front() const { return vector_t{ left(), bottom(), front() }; }
    vector_t right_bottom_front() const { return vector_t{ right(), bottom(), front() }; }
    vector_t left_top_front() const { return vector_t{ left(), top(), front() }; }
    vector_t right_top_front() const { return vector_t{ right(), top(), front() }; }

    vector_t centre;
    vector_t hextent;
};

template <typename T>
bool intersection(const AABB<T, vec3>& A, const vec3<T>& P) {
    return A.left() < P.x && P.x < A.right() &&
           A.bottom() < P.y && P.y < A.top() &&
           A.back() < P.z && P.z < A.front();
}

inline bool intersection(const AABB<int, vec3>& A, const AABB<int, vec3>& B) {
    int r;
    r = A.hextent[0] + B.hextent[0]; if(A.centre[0] - B.centre[0] + r > r + r) return false;
    r = A.hextent[1] + B.hextent[1]; if(A.centre[1] - B.centre[1] + r > r + r) return false;
    r = A.hextent[2] + B.hextent[2]; if(A.centre[2] - B.centre[2] + r > r + r) return false;
    return true;
}

template <typename T>
bool intersection(const AABB<T, vec3>& A, const AABB<T, vec3>& B) {
    if(std::abs(A.centre[0] - B.centre[0]) > (A.hextent[0] + B.hextent[0])) return false;
    if(std::abs(A.centre[1] - B.centre[1]) > (A.hextent[1] + B.hextent[1])) return false;
    if(std::abs(A.centre[2] - B.centre[2]) > (A.hextent[2] + B.hextent[2])) return false;
    return true;
}

template <typename T>
bool intersection(const AABB<T, vec3>& A, const geometry::ray<vec3<T>>& r) {
    return false;
}

using AABB3i = AABB<int, vec3>;
using AABB3f = AABB<float, vec3>;
using AABB3d = AABB<double, vec3>;

}}}

#endif //ZAP_AABB_HPP

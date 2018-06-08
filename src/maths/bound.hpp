//
// Created by otgaard on 2018/06/08.
//

#ifndef ZAP_BOUND_HPP
#define ZAP_BOUND_HPP

#include <cfloat>
#include <vector>
#include <maths/transform.hpp>
#include <maths/geometry/ray.hpp>
#include <maths/geometry/disc.hpp>
#include <maths/geometry/AABB.hpp>
#include <maths/geometry/sphere.hpp>

namespace zap { namespace maths {

template <typename GeoT, typename TransformT>
struct bound {
public:
    using geo_t = GeoT;
    using transform_t = TransformT;
    using affine_t = typename transform_t::affine_t;
    using vector_t = typename transform_t::vec_t;
    using ray_t = geometry::ray<vector_t>;
    using type = typename transform_t::type;

    bound() = default;
    explicit bound(const geo_t& B) : geometry(B) { }
    bound(const bound& rhs) = default;

    bound& operator=(const bound& rhs) = default;
    bound& operator=(const geo_t& geo) {
        this->geometry = geo;
        return *this;
    }

    bound& transform(const vector_t& T);
    bound transform(const vector_t& T) const;
    bound transform_cref(const vector_t& T) const { return transform(T); }
    bound& transform(const transform_t& T);
    bound transform(const transform_t& T) const;
    bound transform_cref(const transform_t& T) const { return transform(T); }
    bound& transform(const affine_t& T);
    bound transform(const affine_t& T) const;
    bound transform_cref(const affine_t& T) const { return transform(T); }

    bound& compute_bound(const std::vector<vector_t>& points) {
        assert(false && "This method requires specialisation");
        return *this;
    }

    bound& grow(const bound& rhs) {
        assert(false && "This method requires specialisation");
        return *this;
    }

    bool intersection(const vector_t& P) const;
    bool intersection(const bound& B) const;
    bool intersection(const ray_t& R) const;
    bool intersection(const vector_t& O, const vector_t& d) const;
    bool intersection(const geo_t& B) const;

    geo_t geometry;
};

template <typename GeoT, typename TransformT>
bound<GeoT, TransformT>& bound<GeoT, TransformT>::transform(const vector_t& T) {
    geometry.translate(T);
    return *this;
}

template <typename GeoT, typename TransformT>
bound<GeoT, TransformT> bound<GeoT, TransformT>::transform(const vector_t& T) const {
    auto copy = *this;
    copy.transform(T);
    return copy;
}

template <typename GeoT, typename TransformT>
bound<GeoT, TransformT>& bound<GeoT, TransformT>::transform(const TransformT& T) {
    geometry.transform(T.affine());
    return *this;
}

template <typename GeoT, typename TransformT>
bound<GeoT, TransformT> bound<GeoT, TransformT>::transform(const TransformT& T) const {
    auto copy = *this;
    copy.transform(T);
    return copy;
}

template <typename GeoT, typename TransformT>
bound<GeoT, TransformT>& bound<GeoT, TransformT>::transform(const typename TransformT::affine_t& T) {
    geometry.transform(T);
    return *this;
}

template <typename GeoT, typename TransformT>
bound<GeoT, TransformT> bound<GeoT, TransformT>::transform(const typename TransformT::affine_t& T) const {
    auto copy = *this;
    copy.transform(T);
    return copy;
}

// Note: Computing the bound requires specialisation
// TODO: Work out a reasonable method for reducing the combinatorial explosion below...

// bound::compute_bound (VS2017 complains if these are not specified as inline)
template <>
inline bound<geometry::disc2f, transform3f>& bound<geometry::disc2f, transform3f>::compute_bound(const std::vector<vec2f>& points) {
    geometry.centre = vec2f(0.f, 0.f);

    for (auto& p : points) geometry.centre += p;
    geometry.centre /= float(points.size());

    geometry.radius = 0.f;
    for (auto& p : points) {
        auto delta = p - geometry.centre;
        auto rsq = delta.length_sqr();
        if (rsq > geometry.radius) geometry.radius = rsq;
    }

    return *this;
}

template <>
inline bound<geometry::AABB2f, transform3f>& bound<geometry::AABB2f, transform3f>::compute_bound(const std::vector<vec2f>& points) {
    if (points.size() == 0) {
        geometry.centre.set(0.f, 0.f);
        geometry.hextent.set(0.f, 0.f);
        return *this;
    }

    auto x_min = std::numeric_limits<float>::max(), x_max = -x_min,
            y_min = x_min,                             y_max = -x_min;

    for (const auto& p : points) {
        if (p.x < x_min) x_min = p.x;
        if (p.x > x_max) x_max = p.x;
        if (p.y < y_min) y_min = p.y;
        if (p.y > y_max) y_max = p.y;
    }

    geometry.hextent.set((x_max - x_min) / 2, (y_max - y_min) / 2);
    geometry.centre.set(x_min + geometry.hextent.x, y_min + geometry.hextent.y);

    return *this;
}

template <>
inline bound<geometry::AABB3f, transform4f>& bound<geometry::AABB3f, transform4f>::compute_bound(const std::vector<vec3f>& points) {
    if (points.size() == 0) {
        geometry.centre.set(0.f, 0.f, 0.f);
        geometry.hextent.set(0.f, 0.f, 0.f);
        return *this;
    }

    auto x_min = std::numeric_limits<float>::max(), x_max = -x_min,
            y_min = x_min,                             y_max = -x_min,
            z_min = x_min,                             z_max = -x_min;

    for (const auto& p : points) {
        if (p.x < x_min) x_min = p.x;
        if (p.x > x_max) x_max = p.x;
        if (p.y < y_min) y_min = p.y;
        if (p.y > y_max) y_max = p.y;
        if (p.z < z_min) z_min = p.z;
        if (p.z > z_max) z_max = p.z;
    }

    geometry.hextent.set((x_max - x_min) / 2, (y_max - y_min) / 2, (z_max - z_min) / 2);
    geometry.centre.set(x_min + geometry.hextent.x, y_min + geometry.hextent.y, z_min + geometry.hextent.z);

    return *this;
}

template <>
inline bound<geometry::AABB2i, transform3i>& bound<geometry::AABB2i, transform3i>::compute_bound(const std::vector<vec2i>& points) {
    if (points.size() == 0) {
        geometry.centre.set(0, 0);
        geometry.hextent.set(0, 0);
        return *this;
    }

    auto x_min = std::numeric_limits<int>::max(), x_max = -x_min,
            y_min = x_min, y_max = -x_min;

    for (const auto& p : points) {
        if (p.x < x_min) x_min = p.x;
        if (p.x > x_max) x_max = p.x;
        if (p.y < y_min) y_min = p.y;
        if (p.y > y_max) y_max = p.y;
    }

    geometry.hextent.set((x_max - x_min) / 2, (y_max - y_min) / 2);
    geometry.centre.set(x_min + geometry.hextent.x, y_min + geometry.hextent.y);

    return *this;
}

template <>
inline bound<geometry::AABB3i, transform4i>& bound<geometry::AABB3i, transform4i>::compute_bound(const std::vector<vec3i>& points) {
    if (points.size() == 0) {
        geometry.centre.set(0, 0, 0);
        geometry.hextent.set(0, 0, 0);
        return *this;
    }

    auto x_min = std::numeric_limits<int>::max(), x_max = -x_min,
            y_min = x_min, y_max = -x_min,
            z_min = x_min, z_max = -x_min;

    for (const auto& p : points) {
        if (p.x < x_min) x_min = p.x;
        if (p.x > x_max) x_max = p.x;
        if (p.y < y_min) y_min = p.y;
        if (p.y > y_max) y_max = p.y;
        if (p.z < z_min) z_min = p.z;
        if (p.z > z_max) z_max = p.z;
    }

    geometry.hextent.set((x_max - x_min) / 2, (y_max - y_min) / 2, (z_max - z_min) / 2);
    geometry.centre.set(x_min + geometry.hextent.x, y_min + geometry.hextent.y, z_min + geometry.hextent.z);

    return *this;
}

// bound::grow
template <>
inline bound<geometry::disc2f, transform3f>& bound<geometry::disc2f, transform3f>::grow(const bound& rhs) {
    if (rhs.geometry.radius == 0) return *this;

    auto delta = rhs.geometry.centre - geometry.centre;
    auto len_sq = delta.length_sqr();
    auto rad_diff = rhs.geometry.radius - geometry.radius;
    auto rd_sq = rad_diff * rad_diff;

    if (rd_sq >= len_sq) {	// Containment
        if (rad_diff >= 0.f) {
            geometry.centre = rhs.geometry.centre;
            geometry.radius = rhs.geometry.radius;
        }
    } else {
        auto len = sqrtf(len_sq);
        if (len > FLT_EPSILON) {
            auto coeff = (len + rad_diff) / (2.f * len);
            geometry.centre += coeff * delta;
        }
        geometry.radius = (len + geometry.radius + rhs.geometry.radius) / 2;
    }

    return *this;
}

template <>
inline bound<geometry::AABB2f, transform3f>& bound<geometry::AABB2f, transform3f>::grow(const bound& rhs) {
    auto x_min = std::numeric_limits<float>::max(), x_max = -x_min,
            y_min = x_min,                             y_max = -x_min;

    x_min = std::min(geometry.left(), rhs.geometry.left());
    x_max = std::max(geometry.right(), rhs.geometry.right());
    y_min = std::min(geometry.bottom(), rhs.geometry.bottom());
    y_max = std::max(geometry.top(), rhs.geometry.top());

    geometry.hextent.set((x_max - x_min) / 2, (y_max - y_min) / 2);
    geometry.centre.set(x_min + geometry.hextent.x, y_min + geometry.hextent.y);
    return *this;
}

template <>
inline bound<geometry::AABB3f, transform4f>& bound<geometry::AABB3f, transform4f>::grow(const bound& rhs) {
    auto x_min = std::numeric_limits<float>::max(), x_max = -x_min,
            y_min = x_min,								y_max = -x_min,
            z_min = x_min,								z_max = -x_min;

    x_min = std::min(geometry.left(), rhs.geometry.left());
    x_max = std::max(geometry.right(), rhs.geometry.right());
    y_min = std::min(geometry.bottom(), rhs.geometry.bottom());
    y_max = std::max(geometry.top(), rhs.geometry.top());
    z_min = std::min(geometry.back(), rhs.geometry.back());
    z_max = std::max(geometry.front(), rhs.geometry.front());

    geometry.hextent.set((x_max - x_min) / 2, (y_max - y_min) / 2, (z_max - z_min) / 2);
    geometry.centre.set(x_min + geometry.hextent.x, y_min + geometry.hextent.y, z_min + geometry.hextent.z);
    return *this;
}

template <>
inline bound<geometry::AABB2i, transform3i>& bound<geometry::AABB2i, transform3i>::grow(const bound& rhs) {
    auto x_min = std::numeric_limits<int>::max(), x_max = -x_min,
            y_min = x_min, y_max = -x_min;

    x_min = std::min(geometry.left(), rhs.geometry.left());
    x_max = std::max(geometry.right(), rhs.geometry.right());
    y_min = std::min(geometry.bottom(), rhs.geometry.bottom());
    y_max = std::max(geometry.top(), rhs.geometry.top());

    geometry.hextent.set((x_max - x_min) / 2, (y_max - y_min) / 2);
    geometry.centre.set(x_min + geometry.hextent.x, y_min + geometry.hextent.y);
    return *this;
}

template <>
inline bound<geometry::AABB3i, transform4i>& bound<geometry::AABB3i, transform4i>::grow(const bound& rhs) {
    auto x_min = std::numeric_limits<int>::max(), x_max = -x_min,
            y_min = x_min, y_max = -x_min,
            z_min = x_min, z_max = -x_min;

    x_min = std::min(geometry.left(), rhs.geometry.left());
    x_max = std::max(geometry.right(), rhs.geometry.right());
    y_min = std::min(geometry.bottom(), rhs.geometry.bottom());
    y_max = std::max(geometry.top(), rhs.geometry.top());
    z_min = std::min(geometry.back(), rhs.geometry.back());
    z_max = std::max(geometry.front(), rhs.geometry.front());

    geometry.hextent.set((x_max - x_min) / 2, (y_max - y_min) / 2, (z_max - z_min) / 2);
    geometry.centre.set(x_min + geometry.hextent.x, y_min + geometry.hextent.y, z_min + geometry.hextent.z);
    return *this;
}

// bound::intersection(vector_t)

template <typename GeoT, typename TransformT>
bool bound<GeoT, TransformT>::intersection(const vector_t& P) const {
    return geometry::intersection(geometry, P);
}

template <typename GeoT, typename TransformT>
bool bound<GeoT, TransformT>::intersection(const bound& B) const {
    return geometry::intersection(geometry, B.geometry);
}

template <typename GeoT, typename TransformT>
bool bound<GeoT, TransformT>::intersection(const ray_t& R) const {
    return geometry::intersection(geometry, R);
}

template <typename GeoT, typename TransformT>
bool bound<GeoT, TransformT>::intersection(const vector_t& O, const vector_t& d) const {
    return intersection(geometry::ray<vector_t>(O, d));
}

template <typename GeoT, typename TransformT>
bool bound<GeoT, TransformT>::intersection(const GeoT& B) const {
    return geometry::intersection(geometry, B);
}

using bound_disc2f = bound<geometry::disc2f, transform3f>;

using bound_AABB2i = bound<geometry::AABB2i, transform3i>;
using bound_AABB3i = bound<geometry::AABB3i, transform4i>;
using bound_AABB2f = bound<geometry::AABB2f, transform3f>;
using bound_AABB3f = bound<geometry::AABB3f, transform4f>;
using bound_AABB2d = bound<geometry::AABB2d, transform3d>;
using bound_AABB3d = bound<geometry::AABB3d, transform4d>;

template struct ZAPMATHS_EXPORT bound<geometry::AABB2i, transform3i>;
template struct ZAPMATHS_EXPORT bound<geometry::AABB3i, transform4i>;
template struct ZAPMATHS_EXPORT bound<geometry::AABB2f, transform3f>;
template struct ZAPMATHS_EXPORT bound<geometry::AABB3f, transform4f>;
template struct ZAPMATHS_EXPORT bound<geometry::AABB2d, transform3d>;
template struct ZAPMATHS_EXPORT bound<geometry::AABB3d, transform4d>;

}}

#endif //ZAP_BOUND_HPP

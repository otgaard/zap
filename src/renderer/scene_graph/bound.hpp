//
// Created by Darren Otgaar on 2016/10/10.
//
/*
#ifndef ZAP_BOUND_HPP
#define ZAP_BOUND_HPP

#include <maths/maths.hpp>
#include <maths/transform.hpp>
#include <maths/geometry/ray.hpp>
#include <maths/geometry/disc.hpp>
#include <maths/geometry/AABB.hpp>
#include <maths/geometry/sphere.hpp>

namespace zap { namespace scene_graph {
    template <typename GeoT, typename TransformT>
    class bound {
    public:
        using geo_t = GeoT;
        using transform_t = TransformT;
        using affine_t = typename transform_t::affine_t;
        using vec_t = typename transform_t::vec_t;
        using ray_t = maths::geometry::ray<vec_t>;
        using type = typename transform_t::type;

        bound() = default;
        explicit bound(const geo_t& B) : geometry_(B) { }
        bound(const bound& rhs) = default;

        bound& operator=(const bound& rhs) = default;

        bound& transform(const transform_t& T) { return *this; }
        const bound transform(const transform_t& T) const { return *this; }
        bound& transform(const affine_t& A) { return *this; }
        const bound transform(const affine_t& A) const { return *this; }

        template <typename BufferT> bool compute_bound(const BufferT& buffer) { return false; }
        bool combine(const bound& rhs) { return false; }

        bool intersection(const vec_t& P) const;
        bool intersection(const bound& B) const;
        bool intersection(const ray_t& R) const;
        bool intersection(const vec_t& O, const vec_t& d);

    protected:
        geo_t geometry_;
    };
}}

#endif //ZAP_BOUND_HPP
*/
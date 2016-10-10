//
// Created by Darren Otgaar on 2016/10/10.
//

#ifndef ZAP_BOUND_HPP
#define ZAP_BOUND_HPP

#include <maths/maths.hpp>
#include <maths/transform.hpp>

namespace zap { namespace engine {
    template <typename GeoT>
    class bound {
    public:
        using geo_t = GeoT;
        using affine_t = typename geo_t::affine_t;
        using transform_t = maths::transform<affine_t>;
        using vec_t = typename transform_t::vec_t;
        using type = typename transform_t::type;

        bound() = default;
        bound(const geo_t& B);
        bound(const bound& rhs) = default;

        bound& operator=(const bound& rhs) = default;

        bound& transform(const transform_t& T);
        const bound transform(const transform_t& T) const;
        bound& transform(const affine_t& A);
        const bound transform(const affine_t& A) const;

    protected:
        geo_t geometry_;
    };
}}

#endif //ZAP_BOUND_HPP

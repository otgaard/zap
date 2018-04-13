//
// Created by Darren Otgaar on 2017/08/22.
//

#ifndef ZAP_GEOMETRY_TRAITS_HPP
#define ZAP_GEOMETRY_TRAITS_HPP

#include <vector>
#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <engine/mesh.hpp>
#include <engine/engine.hpp>

namespace zap { namespace generators {
    using zap::engine::primitive_type;

    template <typename VertexT, primitive_type Primitive>
    struct geometry_traits {
        template <typename T> using vec2 = zap::maths::vec2<T>;
        template <typename T> using vec3 = zap::maths::vec3<T>;
        using vec2f = zap::maths::vec2f;
        using vec3f = zap::maths::vec3f;

        constexpr static primitive_type primitive = Primitive;

        template <typename IndexT>
        static auto make_mesh(const std::tuple<std::vector<VertexT>, std::vector<IndexT>>& m) {
            return engine::make_mesh<VertexT, IndexT>(m);
        }

        static auto make_mesh(const std::vector<VertexT>& m) {
            return engine::make_mesh<VertexT>(m);
        }
    };
}}

#endif //ZAP_GEOMETRY_TRAITS_HPP

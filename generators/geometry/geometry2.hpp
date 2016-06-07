//
// Created by Darren Otgaar on 2016/06/04.
//

#ifndef ZAP_GEOMETRY2_HPP
#define ZAP_GEOMETRY2_HPP

#include <maths/vec2.hpp>
#include <geometry/disc.hpp>

#define ENABLE_LOGGING
#include <tools/log.hpp>

namespace zap { namespace generators {
    using prim_t = zap::engine::primitive_type;
    using maths::vec2;

    template <typename Vertex, prim_t PrimT>
    class geometry2 {
    public:
        constexpr static prim_t primitive = PrimT;
        template <typename T> static std::vector<Vertex> make_disc(const vec2<T>& C, T radius, size_t slices) {
            // Assume either LINE_LOOP or TRI_FAN (intended to be specialised if generic cost is too great)
            auto vertex_count = primitive == prim_t::PT_TRIANGLE_FAN ? slices+2 : slices;
            std::vector<Vertex> vertices(vertex_count);
            const T dt = T(2.0*zap::maths::PI)/slices;
            const auto end = primitive == prim_t::PT_TRIANGLE_FAN ? slices+1 : slices;
            const size_t texcoord = Vertex::find(engine::attribute_type::AT_TEXCOORD1);

            size_t idx = 0;
            if(primitive == prim_t::PT_TRIANGLE_FAN) {
                vertices[idx].position = C;
                if(texcoord != INVALID_IDX) vertices[idx].set(texcoord, vec2<T>(T(.5), T(.5)));
                idx++;
            }

            T theta = 0;
            for(size_t i = 0; i != end; ++i) {
                theta = (i % slices) * dt;
                T stheta = std::sin(theta), ctheta = std::cos(theta);
                vertices[idx].position = vec2<T>(radius * ctheta, radius * stheta);
                if(texcoord != INVALID_IDX) vertices[idx].set(texcoord, vec2<T>(T(.5) + T(.5)*ctheta, T(.5) + T(.5)*stheta));
                idx++;
            }

            return vertices;
        }
    };
}}

#endif //ZAP_GEOMETRY2_HPP

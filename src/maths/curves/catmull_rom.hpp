//
// Created by otgaard on 2018/10/06.
//

#ifndef ZAP_CATMULL_ROM_HPP
#define ZAP_CATMULL_ROM_HPP

#include <vector>
#include <maths/matrix.hpp>

/*
 * Catmull-Rom Spline
 */

namespace zap { namespace maths { namespace curves {
    template <typename VecT>
    struct catmull_rom_spline {
        using type = typename VecT::type;
        using vector_t = VecT;
        using vertices_t = std::vector<vector_t>;
        using times_t = std::vector<type>;
        using U_t = matrix<type, 1, 4>;
        using M_t = matrix<type, 4, 4>;
        using G_t = matrix<vector_t, 4, 1>;

        const static M_t M;

        catmull_rom_spline() = default;
        catmull_rom_spline(const vertices_t& vertices, bool loop=false) : vertices(vertices) {
            assert(vertices.size() >= 3 && "Catmull-Rom requires at least 3 vertices");
            is_looped = loop;
            const auto count = is_looped ? vertices.size() : vertices.size();
            times.resize(count);
            const auto inv = type(1.) / (is_looped ? vertices.size() : vertices.size()-1);
            auto total = type(0.);
            for(auto& t : times) {
                t = total;
                total = t + inv;
            }
        }
        catmull_rom_spline(const vertices_t& vertices, const times_t& times) : vertices(vertices), times(times) {
            is_looped = eq(vertices[0], vertices.back());
        }

        VecT pos(type u) const {
            const size_t count = vertices.size();
            assert(count >= 3 && "Catmull-Rom requires at least 3 vertices");
            const auto idx = find_interval_idx(times, u, is_looped);
            const auto last = is_looped && idx == times.size()-1 ? type(1.) : times[idx+1];
            u = (u - times[idx])/(last - times[idx]);
            const U_t U = {{u*u*u, u*u, u, type(1)}};
            G_t G;
            if(is_looped) {
                G = G_t{{
                    vertices[idx == 0 ? count-1 : idx-1],
                    vertices[idx],
                    vertices[(idx+1)%count],
                    vertices[(idx+2)%count]
                }};
            } else if(idx == 0) {
                G = G_t{{
                    vertices[1] - (vertices[2] - vertices[1]),
                    vertices[0],
                    vertices[1],
                    vertices[2]
                }};
            } else if(idx == count-2) {
                G = G_t{{
                    vertices[idx-1],
                    vertices[idx],
                    vertices[idx+1],
                    vertices[idx+1] - (vertices[idx] - vertices[idx+1])
                }};
            } else if(idx < count-2) {
                G = G_t{{
                    vertices[idx-1],
                    vertices[idx],
                    vertices[idx+1],
                    vertices[idx+2]
                }};
            }
            return (U * (M * G))[0];
        }

        vertices_t vertices;
        times_t times;
        bool is_looped = false;
    };

    template <typename VecT>
    const typename catmull_rom_spline<VecT>::M_t catmull_rom_spline<VecT>::M = {{
        typename VecT::type(-.5), typename VecT::type(1.5),  typename VecT::type(-1.5), typename VecT::type(.5),
        typename VecT::type(1.),  typename VecT::type(-2.5), typename VecT::type(2.),   typename VecT::type(-.5),
        typename VecT::type(-.5), typename VecT::type(0.),   typename VecT::type(.5),   typename VecT::type(0.),
        typename VecT::type(0.),  typename VecT::type(1.),   typename VecT::type(0.),   typename VecT::type(0.)
    }};
}}}

#endif //ZAP_CATMULL_ROM_HPP

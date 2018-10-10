//
// Created by otgaard on 2018/10/06.
//

#ifndef ZAP_CATMULL_ROM_HPP
#define ZAP_CATMULL_ROM_HPP

#include <vector>
#include <maths/matrix.hpp>

namespace zap { namespace maths { namespace curves {
    enum class sampling_method {
        SM_UNIFORM,
        SM_ARC_LENGTH,
        SM_MIDPOINT,
        SM_FAST_CUBIC
    };

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
        catmull_rom_spline(const vertices_t& vertices, bool loop=false) : vertices(vertices), looped(loop) {
            assert(vertices.size() >= 3 && "Catmull-Rom requires at least 3 vertices");
            normalise_uniform();
        }

        // Assign every curve segment a uniform speed
        void normalise_uniform() {
            times.resize(vertices.size());
            const auto inv = type(1.) / (looped ? vertices.size() : vertices.size()-1);
            auto total = type(0.);
            for(auto& t : times) {
                t = total;
                total = t + inv;
            }
        }

        // Normalise curve segments by speed (simple polygonal approximation - use Gaussian integration?)
        void normalise_distance(int steps=10, sampling_method method=sampling_method::SM_UNIFORM) {
            times.resize(vertices.size());
        }

        type distance(type u) const {
            u = clamp(u, type(0), type(1));
            return times[find_interval_idx(times, u)];
        }

        VecT deriv1(type u, type step=type(.01)) {
            return (type(1.)/step) * (pos(u+step) - pos(u));
        }

        VecT pos(type u) const {
            u = clamp(u, type(0), type(1));
            const auto count = vertices.size();
            assert(count >= 3 && "Catmull-Rom requires at least 3 vertices");
            const auto idx = find_interval_idx(times, u);
            const auto end = looped && idx == times.size()-1 ? type(1.) : times[idx+1];
            u = (u - times[idx])/(end - times[idx]);
            const U_t U = {{u*u*u, u*u, u, type(1)}};
            G_t G;
            if(looped) {
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
        bool looped = false;
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

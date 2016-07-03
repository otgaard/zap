//
// Created by Darren Otgaar on 2016/07/03.
//

#ifndef ZAP_HERMITE_HPP
#define ZAP_HERMITE_HPP

#include "maths/maths.hpp"
#include "maths/matrix.hpp"

/* Hermite Curve, uses the matrix form
 * Curve definition (via substitution of PO, T0, P1 and T1 into au^3 + bu^2 + cu + D is
 * Q(u) = (2u^3 - 3u^2 + 1)P0 + (-2u^3 + 3u^2)P1 + (u^3 - 2u^2 + u)T0 + (u^3 - u^2)T1
 *
 * Matrix Form
 *
 * U = [u^3  u^2  u  1]
 * M = [ 2 -2  1  1]
 *     [-3  3 -2 -1]
 *     [ 0  0  1  0]
 *     [ 1  0  0  0]
 * G = [ P0 ]
 *     [ P1 ]
 *     [ T0 ]
 *     [ T1 ]
 *
 * Q(u) = UMG
 */
namespace zap { namespace maths { namespace curves {
    template <typename T, typename VecT>
    struct hermite {
        using type = T;
        using U_t = matrix<T, 1, 4>;
        using M_t = matrix<T, 4, 4>;
        using G_t = matrix<VecT, 4, 1>;

        G_t G;
        const static M_t M;

        hermite() { }
        hermite(const VecT& P0, const VecT& P1, const VecT& T0, const VecT& T1) : G({P0, P1, T0, T1}) { }

        void set_points(const VecT& P0, const VecT& P1) { G[0] = P0; G[1] = P1; }
        void set_tangents(const VecT& T0, const VecT& T1) { G[2] = T0; G[3] = T1; }

        VecT operator()(const T& u) const {
            const U_t U = {{u*u*u, u*u, u, T(1)}};
            return (U * (M * G))[0];
        }

        VecT polynomial(const T& u) const {
            const T u3 = u*u*u; const T u2 = u*u;
            return (2*u3 - 3*u2 + 1)*G[0] + (-2*u3 + 3*u2)*G[1] + (u3 - 2*u2 + u)*G[2] + (u3 - u2)*G[3];
        }
    };

    template <typename T, typename VecT>
    const typename hermite<T,VecT>::M_t hermite<T,VecT>::M = {{
        T( 2.), T(-2.), T( 1.), T( 1.),
        T(-3.), T( 3.), T(-2.), T(-1.),
        T( 0.), T( 0.), T( 1.), T( 0.),
        T( 1.), T( 0.), T( 0.), T( 0.)
    }};

}}}

#endif //ZAP_HERMITE_HPP

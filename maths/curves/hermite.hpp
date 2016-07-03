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
    template <typename T>
    class hermite {
        using type = T;
        using matrix_t = matrix<vec3f, 3, 3>;
    };

}}}

#endif //ZAP_HERMITE_HPP

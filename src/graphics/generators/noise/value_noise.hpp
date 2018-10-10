//
// Created by Darren Otgaar on 2016/06/21.
//

#ifndef ZAP_VALUE_NOISE_HPP
#define ZAP_VALUE_NOISE_HPP

#include "noise.hpp"

namespace zap { namespace generators {
    template <typename T>
    struct value_noise {
        using type_t = T;

        static T noise(T x) {
            int xi = noise::floor(x);
            T dx = x - xi;
            return maths::lerp(noise::grad_i(xi), noise::grad_i(xi+1), dx);
        }

        static T noise(T x, T y) {
            int xi = noise::floor(x), yi = noise::floor(y);
            T dx = x - xi, dy = y - yi;
            return maths::bilinear(noise::grad_i(xi, yi), noise::grad_i(xi+1, yi),
                                   noise::grad_i(xi,yi+1), noise::grad_i(xi+1,yi+1),
                                   dx, dy);
        }

        static T noise(T x, T y, T z) {
            int xi = noise::floor(x), yi = noise::floor(y), zi = noise::floor(z);
            T dx = x - xi, dy = y - yi, dz = z - zi;
            return maths::trilinear(
                noise::grad_i(xi, yi, zi),         noise::grad_i(xi + 1, yi, zi),
                noise::grad_i(xi, yi + 1, zi),     noise::grad_i(xi + 1, yi + 1, zi),
                noise::grad_i(xi, yi, zi + 1),     noise::grad_i(xi + 1, yi, zi + 1),
                noise::grad_i(xi, yi + 1, zi + 1), noise::grad_i(xi + 1, yi + 1, zi + 1),
                dx, dy, dz);
        }
    };
}}

#endif //ZAP_VALUE_NOISE_HPP

//
// Created by Darren Otgaar on 2016/06/24.
//

#ifndef ZAP_PERLIN_HPP
#define ZAP_PERLIN_HPP

#include "noise.hpp"
#include <maths/maths.hpp>
#include <maths/rand_lcg.hpp>

// Based on Ken Perlin's implementation at https://mrl.nyu.edu/~perlin/doc/oscar.html

// Perlin noise ranges:
// 1D = [-.5f, +.5f]                                    // Observed
// 2D = [-sqrt(2)/2, +sqrt(2)/2] = [-.7071f, +.7071f]
// 3D = [-sqrt(2)/2, +sqrt(2)/2] = [-.7071f, +.7071f]   // Observed: should be sqrt(3)/3 ?

namespace zap { namespace generators {
    template <typename T>
    struct perlin {
        using type_t = T;

        static T noise(T x) {
            using namespace maths;

            int bx0, bx1;
            float rx0, rx1, sx, t, u, v;

            t = x + noise::mask;
            bx0 = ((int)t) & noise::mask;
            bx1 = (bx0+1) & noise::mask;
            rx0 = t - (int)t;
            rx1 = rx0 - T(1);

            sx = easing_curve(rx0);
            u = rx0 * noise::grad(bx0);
            v = rx1 * noise::grad(bx1);
            return lerp(sx, u, v);
        }

        static T noise(T x, T y) {
            using namespace maths;
            float sx, sy, a, b, u, v;

            maths::vec2f pos(x + noise::mask, y + noise::mask);
            maths::vec2i b0(noise::floor(pos.x) & noise::mask, noise::floor(pos.y) & noise::mask);
            maths::vec2i b1((b0.x + 1) & noise::mask, (b0.y + 1) & noise::mask);
            maths::vec2f r0(pos.x - noise::floor(pos.x), pos.y - noise::floor(pos.y));
            maths::vec2f r1 = r0 - maths::vec2f(1,1);

            sx = easing_curve(r0.x);
            sy = easing_curve(r0.y);

            u = dot(noise::grad(b0.x, b0.y), r0);
            v = dot(noise::grad(b1.x, b0.y), r1.x, r0.y);
            a = lerp(sx, u, v);

            u = dot(noise::grad(b0.x, b1.y), r0.x, r1.y);
            v = dot(noise::grad(b1.x, b1.y), r1);
            b = lerp(sx, u, v);

            return lerp(sy, a, b);
        }

        static T noise(T x, T y, T z) {
            using namespace maths;
            float sx, sy, sz, a, b, c, d, u, v;

            maths::vec3f pos(x + noise::mask, y + noise::mask, z + noise::mask);
            maths::vec3i b0(noise::floor(pos.x) & noise::mask, noise::floor(pos.y) & noise::mask, noise::floor(pos.z) & noise::mask);
            maths::vec3i b1((b0.x + 1) & noise::mask, (b0.y + 1) & noise::mask, (b0.z + 1) & noise::mask);
            maths::vec3f r0(pos.x - noise::floor(pos.x), pos.y - noise::floor(pos.y), pos.z - noise::floor(pos.z));
            maths::vec3f r1 = r0 - maths::vec3f(1,1,1);

            sx = easing_curve(r0.x);
            sy = easing_curve(r0.y);
            sz = easing_curve(r0.z);

            u = dot(noise::grad(b0.x, b0.y, b0.z), r0);
            v = dot(noise::grad(b1.x, b0.y, b0.z), r1.x, r0.y, r0.z);
            a = lerp(sx, u, v);

            u = dot(noise::grad(b0.x, b1.y, b0.z), r0.x, r1.y, r0.z);
            v = dot(noise::grad(b1.x, b1.y, b0.z), r1.x, r1.y, r0.z);
            b = lerp(sx, u, v);

            c = lerp(sy, a, b);

            u = dot(noise::grad(b0.x, b0.y, b1.z), r0.x, r0.y, r1.z);
            v = dot(noise::grad(b1.x, b0.y, b1.z), r1.x, r0.y, r1.z);
            a = lerp(sx, u, v);

            u = dot(noise::grad(b0.x, b1.y, b1.z), r0.x, r1.y, r1.z);
            v = dot(noise::grad(b1.x, b1.y, b1.z), r1);
            b = lerp(sx, u, v);

            d = lerp(sy, a, b);
            return lerp(sz, c, d);
        }

        static inline T easing_curve(T value) { return value * value * (T(3) - T(2) * value); }
    };

}}

#endif //ZAP_PERLIN_HPP

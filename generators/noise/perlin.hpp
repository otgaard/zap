//
// Created by Darren Otgaar on 2016/06/24.
//

#ifndef ZAP_PERLIN_HPP
#define ZAP_PERLIN_HPP

#include "noise.hpp"
#include <maths/rand_lcg.hpp>

// Based on Ken Perlin's implementation at https://mrl.nyu.edu/~perlin/doc/oscar.html

namespace zap { namespace generators {
    template <typename T>
    struct perlin {
        T fractal(T x, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += ampl*noise1(x*freq);
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

        // Output approximately in range [-.5, .5] - requires clamping
        T fractal(T x, T y, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += ampl*noise2(x*freq, y*freq);
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

        T fractal(T x, T y, T z, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += ampl*noise3(x*freq, y*freq, z*freq);
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

        T turbulence(T x, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += maths::abs(ampl*noise1(x*freq));
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

        // Output in range [0, .5]
        T turbulence(T x, T y, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += maths::abs(ampl*noise2(x*freq, y*freq));
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

        // Output in range [0, .5]
        T turbulence(T x, T y, T z, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += maths::abs(ampl*noise3(x*freq, y*freq, z*freq));
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

        T noise1(T arg) {
            int bx0, bx1;
            float rx0, rx1, sx, t, u, v;

            t = arg + noise::mask;
            bx0 = ((int)t) & noise::mask;
            bx1 = (bx0+1) & noise::mask;
            rx0 = t - (int)t;
            rx1 = rx0 - T(1);

            sx = easing_curve(rx0);
            u = rx0 * noise::grad(bx0);
            v = rx1 * noise::grad(bx1);
            return lerp(sx, u, v);
        }

        T noise2(T x, T y) {
            T v[2] = { x, y };
            return noise2(v);
        }

        T noise2(T vec[2]) {
            float sx, sy, a, b, u, v;

            maths::vec2f pos(vec[0] + noise::mask, vec[1] + noise::mask);
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

        T noise3(T x, T y, T z) {
            T v[3] = { x, y, z };
            return noise3(v);
        }

        T noise3(T vec[3]) {
            float sx, sy, sz, a, b, c, d, u, v;

            maths::vec3f pos(vec[0] + noise::mask, vec[1] + noise::mask, vec[2] + noise::mask);
            maths::vec3i b0(noise::floor(pos.x) & noise::mask, noise::floor(pos.y) & noise::mask, noise::floor(pos.z) & noise::mask);
            maths::vec3i b1((b0.x + 1) & noise::mask, (b0.y + 1) & noise::mask, (b0.z + 1) & noise::mask);
            maths::vec3f r0(pos.x - noise::floor(pos.x), pos.y - noise::floor(pos.y), pos.z - noise::floor(pos.z));
            maths::vec3f r1 = r0 - maths::vec3f(1,1,1);

            sx  = easing_curve(r0.x);
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
        static inline T lerp(T value, T a, T b) { return a + value * (b - a); }
    };

}}

#endif //ZAP_PERLIN_HPP

//
// Created by Darren Otgaar on 2016/06/21.
//

#ifndef ZAP_VALUE_NOISE_HPP
#define ZAP_VALUE_NOISE_HPP

#include "noise.hpp"

namespace zap { namespace generators {
    template <typename T>
    class value_noise {
    public:
        value_noise(ulonglong seed = 1) {
            maths::rand_lcg rand(seed);
            for(size_t i = 0; i != noise::prn_tbl_size; ++i) noise_tbl_[i] = T(1) - T(2) * rand.random();
        }

        T noise_i(int x) const { return noise_tbl_[noise::perm(x)]; }
        T noise_i(int x, int y) const { return noise_tbl_[noise::perm(x,y)]; }
        T noise_i(int x, int y, int z) const { return noise_tbl_[noise::perm(x,y,z)]; }

        T noise(T x) const {
            int xi = noise::floor(x);
            T dx = x - xi;
            return maths::lerp(noise_i(xi), noise_i(xi+1), dx);
        }

        T noise(T x, T y) const {
            int xi = noise::floor(x), yi = noise::floor(y);
            T dx = x - xi, dy = y - yi;
            return maths::bilinear(dx, dy, noise_i(xi, yi), noise_i(xi+1, yi), noise_i(xi,yi+1), noise_i(xi+1,yi+1));
        }

        T noise(T x, T y, T z) const {
            int xi = noise::floor(x), yi = noise::floor(y), zi = noise::floor(z);
            T dx = x - xi, dy = y - yi, dz = z - zi;
            return maths::lerp(maths::bilinear(dx, dy, noise_i(xi, yi, zi), noise_i(xi+1, yi, zi),
                                                       noise_i(xi,yi+1,zi), noise_i(xi+1,yi+1,zi)),
                               maths::bilinear(dx, dy, noise_i(xi, yi, zi+1), noise_i(xi+1, yi, zi+1),
                                                       noise_i(xi,yi+1,zi+1), noise_i(xi+1,yi+1,zi+1)),
                               dz);
        }

        // Output in range [-1, 1]
        T fractal(T x, T y, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += ampl*noise(x*freq, y*freq);
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
                accum += ampl*noise(x*freq, y*freq, z*freq);
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

        // Output in range [0, 1]
        T turbulence(T x, T y, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += maths::abs(ampl*noise(x*freq, y*freq));
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

        T turbulence(T x, T y, T z, byte octaves, T persistence=T(.5), T lacunarity=T(2)) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T mag = T(0);   // Limit is 2
            for(byte i = 0; i != octaves; ++i) {
                accum += maths::abs(ampl*noise(x*freq, y*freq, z*freq));
                mag += ampl;
                ampl *= persistence;
                freq *= lacunarity;
            }
            return (T(1)/mag)*accum;
        }

    protected:
        T noise_tbl_[noise::prn_tbl_size];
    };


}}

#endif //ZAP_VALUE_NOISE_HPP

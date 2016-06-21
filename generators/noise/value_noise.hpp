//
// Created by Darren Otgaar on 2016/06/21.
//

#ifndef ZAP_VALUE_NOISE_HPP
#define ZAP_VALUE_NOISE_HPP

#include <maths/maths.hpp>
#include <maths/rand_lcg.hpp>

namespace zap { namespace generators {
    struct value_noise_defs {
        constexpr static size_t prn_tbl_size = 256;
        constexpr static size_t mask = prn_tbl_size - 1;
        static const byte prn_tbl[prn_tbl_size];
    };

    template <typename T>
    class value_noise {
    public:
        value_noise(ulonglong seed = 1) {
            maths::rand_lcg rand(seed);
            for(size_t i = 0; i != value_noise_defs::prn_tbl_size; ++i) noise_tbl_[i] = T(1) - T(2) * rand.random();
        }

        T noise_i(int x) const { return noise_tbl_[perm(x)]; }
        T noise_i(int x, int y) const { return noise_tbl_[perm(x,y)]; }
        T noise_i(int x, int y, int z) const { return noise_tbl_[perm(x,y,z)]; }

        T noise(T x) const {
            int xi = floor(x);
            T dx = x - xi;
            return maths::lerp(noise_i(xi), noise_i(xi+1), dx);
        }

        T noise(T x, T y) const {
            int xi = floor(x), yi = floor(y);
            T dx = x - xi, dy = y - yi;
            return maths::bilinear(dx, dy, noise_i(xi, yi), noise_i(xi+1, yi), noise_i(xi,yi+1), noise_i(xi+1,yi+1));
        }

        T fractal(T x, T y, byte octaves) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T div = T(0);
            for(byte i = 0; i != octaves; ++i) {
                accum += ampl*noise(x*freq, y*freq);
                div += ampl;
                ampl *= T(0.5);
                freq *= T(2);
            }
            return (T(1)/div)*accum;
        }

        T turbulence(T x, T y, byte octaves) {
            T freq = T(1);
            T ampl = T(1);
            T accum = T(0);
            T div = T(0);
            for(byte i = 0; i != octaves; ++i) {
                accum += maths::abs(ampl*noise(x*freq, y*freq));
                div += ampl;
                ampl *= T(0.5);
                freq *= T(2);
            }
            return (T(1)/div)*accum;
        }

    protected:
        int floor(T x) const { return ((int)x - (x < 0 && x != (int)x)); }
        byte perm(int x) const { return value_noise_defs::prn_tbl[x & value_noise_defs::mask]; }
        byte perm(int x, int y) const { return perm(x + perm(y)); }
        byte perm(int x, int y, int z) const { return perm(x + perm(y + perm(z))); }
        byte perm(int x, int y, int z, int w) const { return perm(x + perm(y + perm(z + perm(w)))); }

        float noise_tbl_[value_noise_defs::prn_tbl_size];
    };


}}

#endif //ZAP_VALUE_NOISE_HPP

//
// Created by Darren Otgaar on 2016/06/24.
//

#ifndef ZAP_NOISE_HPP
#define ZAP_NOISE_HPP

#include <maths/maths.hpp>
#include <maths/vec2.hpp>
#include <maths/vec3.hpp>
#include <maths/rand_lcg.hpp>

namespace zap { namespace generators {
    class noise {
    public:
        using vec3f = maths::vec3<float>;
        using vec2f = maths::vec2<float>;
        constexpr static size_t prn_tbl_size = 256;
        constexpr static byte mask = prn_tbl_size - 1;

        template <typename T> static int floor(T x) { return ((int)x - (x < 0 && x != (int)x)); }
        static byte perm(int x) { return noise::prn_tbl[x & noise::mask]; }
        static byte perm(int x, int y) { return perm(x + perm(y)); }
        static byte perm(int x, int y, int z) { return perm(x + perm(y + perm(z))); }
        static byte perm(int x, int y, int z, int w) { return perm(x + perm(y + perm(z + perm(w)))); }

        static void initialise(ulonglong seed=1);

        static float grad(int x) { return grad1[perm(x)]; }
        static vec2f grad(int x, int y) { return grad2[perm(x, y)]; }
        static vec3f grad(int x, int y, int z) { return grad3[perm(x, y, z)]; }

    protected:
        static bool initialised;
        static byte prn_tbl[prn_tbl_size];
        static vec3f grad3[prn_tbl_size];
        static vec2f grad2[prn_tbl_size];
        static float grad1[prn_tbl_size];
    };
}}

#endif //ZAP_NOISE_HPP

//
// Created by Darren Otgaar on 2016/06/21.
//

#include "noise.hpp"
#include <random>
#include <algorithm>

namespace zap { namespace generators {
    bool noise::initialised = false;

    byte noise::prn_tbl[noise::prn_tbl_size];
    float noise::grad1[noise::prn_tbl_size];
    maths::vec2f noise::grad2[noise::prn_tbl_size];
    maths::vec3f noise::grad3[noise::prn_tbl_size];

    void noise::initialise(ulonglong seed) {
        maths::rand_lcg prn(seed);

        for(size_t i = 0; i != noise::prn_tbl_size; ++i) {
            prn_tbl[i] = byte(i);
            grad1[i] = prn.random_s();
            grad2[i] = normalise(vec2f(prn.random_s(), prn.random_s()));
            grad3[i] = normalise(vec3f(prn.random_s(), prn.random_s(), prn.random_s()));
        }

        //std::random_device rd;
        std::minstd_rand rd;
        std::mt19937 g(rd());

        std::shuffle(prn_tbl, prn_tbl+noise::prn_tbl_size, g);
        initialised = true;
    }
}}
//
// Created by Darren Otgaar on 2016/06/21.
//

#ifndef ZAP_RAND_LCG_HPP
#define ZAP_RAND_LCG_HPP

#include <maths/maths.hpp>

namespace zap { namespace maths {
    class rand_lcg {
    public:
        constexpr static ulonglong modA = 2862933555777941757LL;
        rand_lcg(ulonglong seed=1) : curr_(seed) { }

        uint rand() const {
            curr_ = curr_ * modA + 1;
            return uint(curr_ >> 32);
        }

        float random() const {
            uint flt_i = 0x3f800000 | (rand() >> 9);    // [1 .. 2]
            return *(float*)(&flt_i) - 1.f;             // [0 .. 1]
        }

        float random_s() const {        // Signed random
            uint flt_i = 0x3f800000 | (rand() >> 9);
            return 2*((*(float*)(&flt_i)) - 1.5f);      // [-1 .. 1]
        }

    private:
        mutable ulonglong curr_;
    };

}}

#endif //ZAP_RAND_LCG_HPP

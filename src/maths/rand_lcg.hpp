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
            char* ptr = (char*)&flt_i;
            return *(float*)ptr - 1.f;                  // [0 .. 1]
        }

        float random_s() const {        // Signed random
            uint flt_i = 0x3f800000 | (rand() >> 9);
            char* ptr = (char*)&flt_i;
            return 2 * (*(float*)ptr - 1.5f);           // [-1 .. 1]
        }

        template<class RndIterator>
        void shuffle(RndIterator begin, RndIterator end) {
            for(size_t n = end - begin, i = n - 1; i > 0; --i) {
                std::swap(begin[i], begin[rand()%(i+1)]);
            }
        }

    private:
        mutable ulonglong curr_;
    };

}}

#endif //ZAP_RAND_LCG_HPP

/* Created by Darren Otgaar on 2016/03/05. http://www.github.com/otgaard/zap */
#ifndef ZAP_VEC_HPP
#define ZAP_VEC_HPP

#include "maths.hpp"

namespace zap { namespace maths {
    template <typename T, size_t N>
    class vec {
    public:
        constexpr static size_t size() { return N; }
        constexpr static size_t memsize() { return sizeof(T)*N; }

        inline vec() { }
        inline explicit vec(T v) { for(auto& i : v_) i = v; }
        inline vec(const std::initializer_list<T> lst) {
            assert(lst.size() == N && ZERR_ARG_COUNT_INVALID);
            std::copy(lst.begin(), lst.end(), v_);
        }
        inline vec(const T* v, size_t C) { std::copy(v, v+C, v_); }

    protected:
        T v_[N];
    };
}}

#endif //ZAP_VEC_HPP

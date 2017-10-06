//
// Created by Darren Otgaar on 2016/06/22.
//

#ifndef ZAP_CURVES_HPP
#define ZAP_CURVES_HPP

#include "maths/maths.hpp"

namespace zap { namespace maths { namespace curves {
    template <typename T, typename FncX, typename FncY>
    struct curve2 {
        constexpr static auto make_fnc(FncX fncX, FncY fncY) {
            return [=](const T& u) {
                return vec2<T>(fncX(u), fncY(u));
            };
        }
    };

}}}

#endif //ZAP_CURVES_HPP

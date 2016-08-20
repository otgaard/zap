//
// Created by Darren Otgaar on 2016/08/20.
//

#ifndef ZAP_RECT_HPP
#define ZAP_RECT_HPP

#include "maths.hpp"

namespace zap { namespace maths {
    template <typename T>
    struct rect {
        T left, bottom, right, top;
    };
}}

#endif //ZAP_RECT_HPP

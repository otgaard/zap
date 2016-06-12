//
// Created by Darren Otgaar on 2016/06/11.
//

#ifndef ZAP_COLOUR_HPP
#define ZAP_COLOUR_HPP

#include <maths/vec3.hpp>

struct colour {
    using vec3b = zap::maths::vec3b;

    constexpr static vec3b black8 = vec3b(0, 0, 0);
    constexpr static vec3b white8 = vec3b(255, 255, 255);
    constexpr static vec3b red8 = vec3b(255, 0, 0);
    constexpr static vec3b green8 = vec3b(0, 255, 0);
    constexpr static vec3b blue8 = vec3b(0, 0, 255);
    constexpr static vec3b yellow8 = vec3b(255, 255, 0);
    constexpr static vec3b purple8 = vec3b(255, 0, 255);
    constexpr static vec3b cyan8 = vec3b(0, 255, 255);

};

#endif //ZAP_COLOUR_HPP

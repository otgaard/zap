//
// Created by Darren Otgaar on 2017/02/04.
//

#ifndef ZAP_PROCGEN1_GENERATORS_HPP
#define ZAP_PROCGEN1_GENERATORS_HPP

#include <maths/vec2.hpp>
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>

zap::engine::pixmap<zap::engine::rgba8888_t> make_radial(int width, int height) {
    using namespace zap::maths;
    using namespace zap::engine;

    const float inv_w = 1.f/width, inv_h = 1.f/height;
    pixmap<rgba8888_t> img(width, height);
    for(int r = 0; r != height; ++r) {
        auto y = .5f - r*inv_h;
        for(int c = 0; c != width; ++c) {
            auto x = .5f - c*inv_w;
            auto s = std::max(1.f - 2.f*std::sqrtf(x*x + y*y), 0.f);
            auto v = std::round(s*255);
            img(c,r).set4(v,v,v,v);
        }
    }

    return img;
}

#endif //ZAP_PROCGEN1_GENERATORS_HPP

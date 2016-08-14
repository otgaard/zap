/* Created by Darren Otgaar on 2016/08/07. http://www.github.com/otgaard/zap */
#ifndef ZAP_TRACER_HPP
#define ZAP_TRACER_HPP

#include <vector>
#include <engine/pixel_format.hpp>

// Motivates the addition of geometric primitives, intersection tests, spatial partitioning, and PBR models.

namespace zap { namespace renderer {

class raytracer {
public:
    raytracer();
    ~raytracer();

    std::vector<engine::rgb888_t> render(int w, int h);
};

}}

#endif //ZAP_TRACER_HPP


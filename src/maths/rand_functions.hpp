//
// Created by Darren Otgaar on 2018/04/15.
//

#ifndef ZAP_RAND_FUNCTIONS_HPP
#define ZAP_RAND_FUNCTIONS_HPP

#include <maths/rand_lcg.hpp>
#include <maths/algebra.hpp>

namespace zap { namespace maths {

struct rand_generator {
    rand_generator(uint64_t seed=1) : rnd(seed) { }

    float rand1f(float min=0.f, float max=1.f) {
        return lerp(rnd.random(), min, max);
    }

    vec2f rand2f(const vec2f& min = {0.f, 0.f}, const vec2f& max = {1.f, 1.f}) {
        return vec2f{
            lerp(rnd.random(), min.x, max.x),
            lerp(rnd.random(), min.y, max.y)
        };
    }

    vec3f rand3f(const vec3f& min = {0.f, 0.f, 0.f}, const vec3f& max = {1.f, 1.f, 1.f}) {
        return vec3f{
            lerp(rnd.random(), min.x, max.x),
            lerp(rnd.random(), min.y, max.y),
            lerp(rnd.random(), min.z, max.z)
        };
    }

    vec4f rand4f(const vec4f& min = {0.f, 0.f, 0.f, 0.f}, const vec4f& max = {1.f, 1.f, 1.f, 1.f}) {
        return vec4f{
            lerp(rnd.random(), min.x, max.x),
            lerp(rnd.random(), min.y, max.y),
            lerp(rnd.random(), min.z, max.z),
            lerp(rnd.random(), min.w, max.w)
        };
    }

    byte rand1b(byte min = 0, byte max = 255) {
        return byte(lerp(rnd.random(), min, max));
    }

    vec3b rand3b(const vec3b& min = {0, 0, 0}, const vec3b& max = {255, 255, 255}) {
        return vec3b{
            lerp(rnd.random(), min.x, max.x),
            lerp(rnd.random(), min.y, max.y),
            lerp(rnd.random(), min.z, max.z)
        };
    }

    vec4b rand4b(const vec4b& min = {0, 0, 0, 0}, const vec4b& max = {255, 255, 255, 255}) {
        return vec4b{
                lerp(rnd.random(), min.x, max.x),
                lerp(rnd.random(), min.y, max.y),
                lerp(rnd.random(), min.z, max.z),
                lerp(rnd.random(), min.w, max.w)
        };
    }

    rand_lcg rnd;
};

}}

#endif //ZAP_RAND_FUNCTIONS_HPP

/* Created by Darren Otgaar on 2016/08/07. http://www.github.com/otgaard/zap */
#include "raytracer.hpp"
#include <maths/algebra.hpp>
#include <maths/geometry/ray.hpp>
#include <maths/geometry/plane.hpp>
#include <maths/geometry/sphere.hpp>
#include <maths/io.hpp>

using namespace zap::maths;
using namespace zap::engine;
using namespace zap::renderer;
using namespace zap::maths::geometry;

raytracer::raytracer() {

}

raytracer::~raytracer() {

}

static zap::maths::loop<float> height(-1.8f, 1.6f, -1.f);

std::vector<zap::engine::rgb888_t> raytracer::render(int w, int h) {
    const float inv_w = 1.f/w, inv_h = 1.f/h;

    const float z_min = -.1f, z_max = 100.f, fov = float(PI)/2.f, ar = float(h)/w;
    const float s = -2.f * std::tan(.5f*fov);

    const vec3f light_pos = vec3f(0,2,-4.f);
    sphere<float> sphere1(vec3f(0.f,height.value,-4.f), .2f);
    plane<float> plane1(vec3f(0,-2.f,0.f), vec3f(0,1.f,0));
    ray<vec3f> ray1(vec3f(0,0,0), vec3f(0,0,-1.f));
    vec2f roots;
    int row_offset;
    float t;

    vec2f offsets[4] = {
        {.25f,.25f},
        {.75f,.25f},
        {.75f,.75f},
        {.25f,.75f}
    };

    std::vector<zap::engine::rgb888_t> pixels(w*h, rgb888_t(0,0,0));
    for(int r = 0; r != h; ++r) {
        row_offset = r*w;
        for(int c = 0; c != w; ++c) {
            vec3f pixel(0,0,0);
            for(int sm = 0; sm != 4; ++sm) {
                ray1.O = vec3f(((c + offsets[sm].x)*inv_w -.5f) * s, ((r + offsets[sm].y)*inv_h - .5f) * s * ar, 1.f) * z_min;
                ray1.d = normalise(ray1.O);

                if(int count = intersection(sphere1, ray1, roots.data())) {
                    auto P = ray1.position(count == 2 ? roots.y : roots.x);
                    auto N = normalise(P - sphere1.C);
                    auto ld = normalise(light_pos - P);
                    pixel += vec3f(byte(25+max(dot(ld,N),0.f)*220),0,0);
                } else if((t = intersection(plane1, ray1)) > 0.f && t < z_max) {
                    auto P = ray1.position(t);
                    auto ld = normalise(light_pos - P);
                    if(intersection(sphere1, ray<vec3f>(P, ld), roots.data())) {
                        pixel += vec3f(0, 0, 25);
                    } else {
                        pixel += vec3f(0, 0, byte(max(dot(ld, plane1.n), 0.f) * 255));
                    }
                }
            }
            pixels[row_offset+c].set3(pixel*.25f);
        }
    }

    height += 0.1f;

    return pixels;
}

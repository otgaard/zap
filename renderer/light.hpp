/* Created by Darren Otgaar on 2016/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_LIGHT_HPP
#define ZAP_LIGHT_HPP

#include <maths/algebra.hpp>
#include <engine/uniform_buffer.hpp>

namespace zap { namespace renderer {
    using light_dir_basic = engine::uniform_block<
        core::light_dir<maths::vec4f>,
        core::light_colour<maths::vec4f>,
        core::light_intensity<float>
    >;

    // TODO: Generate these from the input uniform_block for inclusion into the shader
    const char* const light_dir_def =
        "struct dir_light {"
           "vec4 dir;"
           "vec4 colour;"
           "float intensity;"
        "};";

    const char* const light_dir_fnc =
    "light_sample compute_dir_light(int idx) {"
        "light_sample l;"
        "l.L = lights[idx].dir.xyz;"
        "l.i = lights[idx].dir.intensity;"
        "return l;"
    "}";

    using light_point_basic = engine::uniform_block<
        core::light_position<maths::vec4f>,
        core::light_attenuation<maths::vec4f>,
        core::light_colour<maths::vec4f>,
        core::light_intensity<float>
    >;

    const char* const light_point_def =
        "struct point_light {"
            "vec4 position;"
            "vec4 attenuation;"
            "vec4 colour;"
            "float intensity;"
        "};";

    const char* const light_point_fnc =
        "light_sample compute_point_light(vec3 P, int idx) {"
            "light_sample l;"
            "vec3 D =  lights[idx].position.xyz - P;"
            "float d = length(D);"
            "l.L = D/d;"
            "l.i = lights[idx].intensity/dot(vec3(1., d, d*d), lights[idx].attenuation.xyz);"
            "return l;"
        "}";

    using light_spot_basic = engine::uniform_block<
        core::light_position<maths::vec4f>,
        core::light_dir<maths::vec4f>,
        core::light_attenuation<maths::vec4f>,
        core::light_colour<maths::vec4f>,
        core::light_cos_angle<float>,
        core::light_exponent<float>,
        core::light_intensity<float>
    >;

    const char* const light_spot_def =
        "layout(std140) struct spot_light {"
            "vec4 position;"
            "vec4 dir;"
            "vec4 attentuation;"
            "vec4 colour;"
            "float cos_angle;"
            "float exponent;"
            "float intensity;"
        "};";

    const char* const light_spot_fnc =
        "light_sample compute_spot_light(vec3 P, int idx) {"
            "light_sample l;"
            "vec3 D = lights[idx].position.xyz - P;"
            "float d = length(D);"
            "l.L = D/d;"
            "float ct = dot(-l.L, lights[idx].dir.xyz);"
            "l.i = ct > lights[idx].cos_angle"
                "? lights[idx].intensity * pow(ct, lights[idx].exponent)/dot(lights[idx].attentuation.xyz, vec3(1., d, d*d))"
                ": 0.;"
            "return l;"
        "}";

    template <size_t N>
    using lights_dir = engine::uniform_block<
        core::lights_dir<std::array<light_dir_basic, N>>,
        core::light_count<int>
    >;

    template <size_t N>
    using lights_point = engine::uniform_block<
        core::lights_point<std::array<light_point_basic, N>>,
        core::light_count<int>
    >;

    template <size_t N>
    using lights_spot = engine::uniform_block<
        core::lights_spot<std::array<light_spot_basic, N>>,
        core::light_count<int>
    >;



}}

#endif //ZAP_LIGHT_HPP

/* Created by Darren Otgaar on 2016/09/18. http://www.github.com/otgaard/zap */
#ifndef ZAP_MATERIAL_HPP
#define ZAP_MATERIAL_HPP

#include <maths/algebra.hpp>
#include <engine/uniform_buffer.hpp>

namespace zap { namespace renderer {
    using material_block = engine::uniform_block<
        core::material_emissive<maths::vec4f>,
        core::material_ambient<maths::vec4f>,
        core::material_diffuse<maths::vec4f>,
        core::material_specular<maths::vec4f>,
        core::material_exponent<float>
    >;

    const char* const material_def =
        "layout(std140) uniform material {"
            "vec4 material_emissive;"
            "vec4 material_ambient;"
            "vec4 material_diffuse;"
            "vec4 material_specular;"
            "float material_exponent;"
        "};";

    const char* const Me_component_fnc =
        "vec3 compute_Me() {"
            "return material_emissive.rgb;"
        "}";

    const char* const Ma_component_fnc =
        "vec3 compute_Ma(light_sample sample, vec3 La) {"
            "return sample.i * La * material_ambient.rgb;"
        "}";

    const char* const Md_component_fnc =
        "vec3 compute_Md(light_sample sample, vec3 N, vec3 Ld) {"
            "return sample.i * Ld * material_diffuse.rgb * max(dot(N, sample.L), 0.);"
        "}";

    const char* const Ms_component_fnc =
        "vec3 compute_Ms(light_sample sample, vec3 N, vec3 P, vec3 Ls) {"
            "vec3 V = -normalize(P);"
            "vec3 H = normalize(V + sample.L);"
            "return sample.i * Ls * material_specular.rgb * pow(max(dot(N, H), 0.), material_exponent) * max(dot(N, sample.L), 0.);"
        "}";
}}

#endif //ZAP_MATERIAL_HPP

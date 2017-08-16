/* Created by Darren Otgaar on 2017/08/15. http://www.github.com/otgaard/zap */
#ifndef ZAP_SHADER_BUILDER_HPP
#define ZAP_SHADER_BUILDER_HPP

/**
 * The shader_builder builds shaders based on selected features and configuration and returns a master context and
 * the relevant uniform blocks, sampler, and texture binding points.  The shader_builder is used to selectively turn
 * on default lighting, materials, and post-processing features.
 */

#include <string>
#include <engine/program.hpp>
#include <renderer/light.hpp>
#include <renderer/material.hpp>
#include <renderer/render_context.hpp>

namespace zap { namespace renderer {

template <size_t MaxDirLights=4, size_t MaxPointLights=10, size_t MaxSpotLights=4>
struct builder_task {
    static constexpr size_t max_dir_lights = MaxDirLights;
    static constexpr size_t max_point_lights = MaxPointLights;
    static constexpr size_t max_spot_lights = MaxSpotLights;

    enum class lighting_method {
        LM_GOURAUD,
        LM_PHONG,
        LM_BRDF
    } method = lighting_method::LM_PHONG;


};

class shader_builder {
public:

    // Builds a basic light + material shader
    template <size_t MaxDirLights, size_t MaxPointLights, size_t MaxSpotLights>
    static std::unique_ptr<render_context> build_basic_lights(builder_task<MaxDirLights, MaxPointLights, MaxSpotLights>& req) {

    }



};

}}

#endif //ZAP_SHADER_BUILDER_HPP

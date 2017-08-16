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
#include <engine/texture.hpp>
#include <renderer/light.hpp>
#include <renderer/camera.hpp>
#include <renderer/material.hpp>
#include <renderer/render_context.hpp>

namespace zap { namespace renderer {

template <size_t MaxDirLights=4, size_t MaxPointLights=10, size_t MaxSpotLights=4>
struct builder_task {
    using texture_type = engine::texture_type;

    static constexpr size_t max_dir_lights = MaxDirLights;
    static constexpr size_t max_point_lights = MaxPointLights;
    static constexpr size_t max_spot_lights = MaxSpotLights;

    using lights_block_t = lights_block<MaxDirLights, MaxPointLights, MaxSpotLights>;

    enum class lighting_method {
        LM_GOURAUD,
        LM_PHONG,
        LM_BRDF
    } method = lighting_method::LM_PHONG;

    bool material_colour = true;

    texture_type diffuse_map = texture_type::TT_TEX2D;
    bool has_gloss_channel = false;                             // Use the A component in diffuse_map for gloss_map
    texture_type gloss_map = texture_type::TT_NONE;
    texture_type glow_map = texture_type::TT_NONE;
    texture_type bump_map = texture_type::TT_NONE;

    bool use_camera_block = true;       // Default to using the camera uniform block
};

const char* const GLSL_HEADER = "#version 330 core\n";
const char* const GLSL_OPEN_MAIN = "void main() {";
const char* const GLSL_CLOSE_MAIN = "}";

class shader_builder {
public:
    // Builds a basic light + material shader
    template <size_t D, size_t P, size_t S>
    static std::unique_ptr<render_context> build_basic_lights(const builder_task<D, P, S>& req) {
        using task_t = builder_task<D, P, S>;
        term = "\n";    // Terminal char for formatting
        std::string vertex_shdr;
        std::string fragment_shdr;

        vertex_shdr += GLSL_HEADER;
        if(req.method == task_t::lighting_method::LM_GOURAUD) {
            vertex_shdr += add_camera_block(req);
        }

        auto rndr_context = std::make_unique<render_context>(vertex_shdr, fragment_shdr);
        if(!rndr_context->initialise()) {
            LOG_ERR("Failed in build_basic_lights()");
            rndr_context.release();
        }
        return rndr_context;
    }

protected:
    static const char* term;

    template <size_t D, size_t P, size_t S>
    static std::string __build_method__(const builder_task<D, P, S>& req) {
        std::string block;

        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string add_camera_block(const builder_task<D, P, S>& req) {
        std::string block;
        if(req.use_camera_block) block += camera::uniform_block_def(term);
        else assert(false && "Not implemented yet");
        return block;
    }


};

}}

#endif //ZAP_SHADER_BUILDER_HPP

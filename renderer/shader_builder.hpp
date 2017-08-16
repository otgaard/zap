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

    bool is_gouraud() const { return method == lighting_method::LM_GOURAUD; }
    bool is_phong() const { return method == lighting_method::LM_PHONG; }
    bool is_brdf() const { return method == lighting_method::LM_BRDF; }

    bool material_colour = true;

    texture_type diffuse_map = texture_type::TT_TEX2D;
    bool has_gloss_channel = false;                             // Use the A component in diffuse_map for gloss_map
    texture_type gloss_map = texture_type::TT_NONE;
    texture_type glow_map = texture_type::TT_NONE;
    texture_type bump_map = texture_type::TT_NONE;

    bool has_textures() const {
        return diffuse_map != texture_type::TT_NONE ||
               gloss_map != texture_type::TT_NONE ||
               glow_map != texture_type::TT_NONE ||
               bump_map != texture_type::TT_NONE;
    }

    bool use_camera_block = true;       // Default to using the camera uniform block
};

const std::string GLSL_HEADER = "#version 330 core\n";
const std::string GLSL_OPEN_MAIN = "void main() {";
const std::string GLSL_CLOSE_MAIN = "}";

class shader_builder {
public:
    // Builds a basic light + material shader
    template <size_t D, size_t P, size_t S>
    static std::unique_ptr<render_context> build_basic_lights(const builder_task<D, P, S>& req) {
        using task_t = builder_task<D, P, S>;

        assert(!req.is_brdf() && "BRDF shading not supported yet");

        term = "\n";    // Terminal char for formatting
        std::string vertex_shdr;
        std::string fragment_shdr;

        vertex_shdr = build_vertex_shader(req);
        fragment_shdr = build_fragment_shader(req);

        LOG(LOG_YELLOW, vertex_shdr);
        LOG(LOG_CYAN, fragment_shdr);

        auto rndr_context = std::make_unique<render_context>(vertex_shdr, fragment_shdr);
        if(!rndr_context->initialise()) {
            LOG_ERR("Failed in build_basic_lights()");
            rndr_context.release();
        }
        return rndr_context;
    }

protected:
    static std::string term;

    template <size_t D, size_t P, size_t S>
    static std::string __build_method__(const builder_task<D, P, S>& req) {
        std::string block;

        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_transform_block(const builder_task<D, P, S>& req) {
        std::string block;

        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_light_block(const builder_task<D, P, S>& req) {
        std::string block;
        block += light_sample_def + term;
        block += light_dir_def + term;
        block += light_point_def + term;
        block += light_spot_def + term;
        block += "layout(std140) uniform lights {" + term;
        block += "dir_light lights_dir[" + std::to_string(D) + "];" + term;
        block += "point_light lights_point[" + std::to_string(P) + "];" + term;
        block += "spot_light lights_spot[" + std::to_string(S) + "];" + term;
        block += "ivec4 light_count;" + term;
        block += "};" + term;
        block += light_dir_fnc + term;
        block += light_point_fnc + term;
        block += light_spot_fnc + term;
        return block;
    }


    template <size_t D, size_t P, size_t S>
    static std::string build_vertex_shader(const builder_task<D, P, S>& req) {
        std::string block = GLSL_HEADER;
        if(req.is_gouraud() || req.is_phong()) block += build_camera_block(req);

        if(req.is_gouraud()) {
            block += build_light_block(req);

            block += "out vec4 colour;" + term;
        }

        if(req.has_textures()) {
            block += "out vec2 tex2;" + term;       // for 2D textures
            block += "out vec3 tex3;" + term;       // for 3D & Cube maps
        }

        block += GLSL_OPEN_MAIN + term;


        block += GLSL_CLOSE_MAIN + term;

        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_fragment_shader(const builder_task<D, P, S>& req) {
        std::string block = GLSL_HEADER;
        if(req.is_phong()) block += build_camera_block(req);

        block += "out vec4 frag_colour;" + term;
        block += GLSL_OPEN_MAIN + term;
        block += "frag_colour = vec4(1., 1., 1., 1.);" + term;
        block += GLSL_CLOSE_MAIN + term;

        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_camera_block(const builder_task<D, P, S>& req) {
        std::string block;
        if(req.use_camera_block) block += camera::uniform_block_def(term.c_str());
        else assert(req.use_camera_block && "Not implemented yet");
        return block;
    }


};

}}

#endif //ZAP_SHADER_BUILDER_HPP

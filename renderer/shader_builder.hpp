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
        LM_NONE,        // Diffuse colour + texture only
        LM_FLAT,        // Gouraud + flat output
        LM_GOURAUD,
        LM_PHONG,
        LM_BRDF
    } method = lighting_method::LM_PHONG;

    bool is_nolight() const { return method == lighting_method::LM_NONE; }
    bool is_flat() const { return method == lighting_method::LM_FLAT; }
    bool is_gouraud() const { return method == lighting_method::LM_GOURAUD || method == lighting_method::LM_FLAT; }
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

    bool has_diffuse_map() const { return diffuse_map != texture_type::TT_NONE; }
    bool has_gloss_map() const { return gloss_map != texture_type::TT_NONE; }
    bool has_glow_map() const { return glow_map != texture_type::TT_NONE; }
    bool has_bump_map() const { return bump_map != texture_type::TT_NONE; }

    bool use_camera_block = true;       // Default to using the camera uniform block
};

const std::string GLSL_HEADER = "#version 330 core\n";
const std::string GLSL_OPEN_MAIN = "void main() {";
const std::string GLSL_CLOSE_MAIN = "}";

class shader_builder {
public:
    using texture_type = engine::texture_type;

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

        LOG(LOG_YELLOW, term, vertex_shdr);
        LOG(LOG_CYAN, term, fragment_shdr);

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

    // Default to p3n3t2 for now
    template <size_t D, size_t P, size_t S>
    static std::string build_vertex_input(const builder_task<D, P, S>& req) {
        std::string block;
        block += "in vec3 position;" + term;
        block += "in vec3 normal;" + term;
        block += "in vec2 texcoord1;" + term;
        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_transform_block(const builder_task<D, P, S>& req) {
        std::string block;
        if(!req.is_nolight()) {
            block += "uniform mat4 model_matrix;" + term;
            block += "uniform mat4 mv_matrix;" + term;
            block += "uniform mat3 normal_matrix;" + term;
        }
        block += "uniform mat4 mvp_matrix;" + term;
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
        block +=    "dir_light lights_dir[" + std::to_string(D) + "];" + term;
        block +=    "point_light lights_point[" + std::to_string(P) + "];" + term;
        block +=    "spot_light lights_spot[" + std::to_string(S) + "];" + term;
        block +=    "ivec4 light_count;" + term;
        block += "};" + term;
        block += light_dir_fnc + term;
        block += light_point_fnc + term;
        block += light_spot_fnc + term;
        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_material_block(const builder_task<D, P, S>& req) {
        std::string block;
        block += material_def + term;
        block += Me_component_fnc + term;
        block += Ma_component_fnc + term;
        block += Md_component_fnc + term;
        block += Ms_component_fnc + term;
        return block;
    };

    template <size_t D, size_t P, size_t S>
    static std::string build_lighting_output(const builder_task<D, P, S>& req) {
        std::string block;
        block += "out vec3 P;" + term;
        block += "out vec3 nor;" + term;
        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_lighting_vshdr(const builder_task<D, P, S>& req) {
        std::string block;
        block += "P = (mv_matrix * vec4(position, 1.)).xyz;" + term;
        block += "nor = normal_matrix * normal;" + term;
        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_light_computation(const builder_task<D, P, S>& req) {
        std::string block;
        if(req.is_gouraud()) {
            block += "vec3 P = (mv_matrix * vec4(position, 1.)).xyz;" + term;
            block += "vec3 N = normal_matrix * normal;" + term;
        } else if(req.is_phong()) {
            block += "vec3 N = normalize(nor);" + term;
            block += "vec4 colour;" + term;
        }
        block += "vec3 accum = mat.material_emissive.rgb;" + term;

        if(D > 0) {
            block += "for(int i = 0; i != light_count.x; ++i) {" + term;
            block += "light_sample sample = compute_dir_light(i);" + term;
            block += "accum += compute_Ma(sample, lights_dir[i].colour.rgb * lights_dir[i].ADS.x)" + term;
            block += "       + compute_Md(sample, N, lights_dir[i].colour.rgb * lights_dir[i].ADS.y)" + term;
            block += "       + compute_Ms(sample, N, P, lights_dir[i].colour.rgb * lights_dir[i].ADS.z);" + term;
            block += "}" + term;
        }

        if(P > 0) {
            block += "for(int i = 0; i != light_count.y; ++i) {" + term;
            block += "light_sample sample = compute_point_light(P, i);" + term;
            block += "accum += compute_Ma(sample, lights_point[i].colour.rgb * lights_point[i].ADS.x)" + term;
            block += "       + compute_Md(sample, N, lights_point[i].colour.rgb * lights_point[i].ADS.y)" + term;
            block += "       + compute_Ms(sample, N, P, lights_point[i].colour.rgb * lights_point[i].ADS.z);" + term;
            block += "}" + term;
        }

        if(S > 0) {
            block += "for(int i = 0; i != light_count.z; ++i) {" + term;
            block += "light_sample sample = compute_spot_light(P, i);" + term;
            block += "accum += compute_Ma(sample, lights_spot[i].colour.rgb * lights_spot[i].ADS.x)" + term;
            block += "       + compute_Md(sample, N, lights_spot[i].colour.rgb * lights_spot[i].ADS.y)" + term;
            block += "       + compute_Ms(sample, N, P, lights_spot[i].colour.rgb * lights_spot[i].ADS.z);" + term;
            block += "}" + term;
        }

        block += "colour = vec4(accum, mat.material_diffuse.a);" + term;
        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_vertex_shader(const builder_task<D, P, S>& req) {
        std::string block = GLSL_HEADER;

        block += build_vertex_input(req);

        if(req.is_gouraud() || req.is_phong()) block += build_camera_block(req);

        block += build_transform_block(req);

        if(req.is_gouraud()) {
            block += build_light_block(req);
            block += build_material_block(req);
            if(req.is_flat()) block += "flat out vec4 colour;" + term;
            else              block += "out vec4 colour;" + term;
        } else if(req.is_phong()) {
            block += build_lighting_output(req);
        }

        if(req.has_textures()) {
            block += "out vec2 tex2;" + term;       // for 2D textures
            if(req.diffuse_map == texture_type::TT_CUBE_MAP) block += "out vec3 tex3;" + term;
        }

        block += GLSL_OPEN_MAIN + term;
        if(req.is_gouraud()) {
            block += build_light_computation(req);
        } else if(req.is_phong()) {
            block += build_lighting_vshdr(req);
        } else {
            assert(!req.is_brdf() && "Not yet supported");
        }

        if(req.has_textures()) {
            block += "tex2 = texcoord1;" + term;
            if(req.diffuse_map == texture_type::TT_CUBE_MAP) block += "tex3 = normal;" + term;
        }

        block += "gl_Position = mvp_matrix * vec4(position, 1.);" + term;

        block += GLSL_CLOSE_MAIN + term;

        return block;
    }

    // TODO: Need type for sampler
    static std::string sampler_type(texture_type type) {
        switch(type) {
            case texture_type::TT_TEX1D: return "sampler1D";
            case texture_type::TT_TEX2D: return "sampler2D";
            case texture_type::TT_TEX3D: return "sampler3D";
            case texture_type::TT_CUBE_MAP: return "samplerCube";
            default: assert(false && "Unsupported texture type");
        }
        return "";
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_lighting_input(const builder_task<D, P, S>& req) {
        std::string block;
        block += "in vec3 P;" + term;
        block += "in vec3 nor;" + term;
        return block;
    }

    template <size_t D, size_t P, size_t S>
    static std::string build_fragment_shader(const builder_task<D, P, S>& req) {
        std::string block = GLSL_HEADER;
        if(req.has_textures()) {
            block += "in vec2 tex2;" + term;
            if(req.diffuse_map == texture_type::TT_CUBE_MAP) block += "in vec3 tex3;" + term;
        }
        if(req.is_gouraud()) {
            if(req.is_flat()) block += "flat in vec4 colour;" + term;
            else              block += "in vec4 colour;" + term;
        } else if(req.is_phong()) {
            block += build_camera_block(req);
            block += build_lighting_input(req);
        } else {
            assert(!req.is_brdf() && "Not yet supported");
        }

        if(req.has_textures()) {
            if(req.has_diffuse_map())  block += "uniform " + sampler_type(req.diffuse_map) + " diffuse_map;" + term;
            if(req.has_gloss_map()) block += "uniform " + sampler_type(req.gloss_map) + " gloss_map;" + term;
            if(req.has_glow_map())  block += "uniform " + sampler_type(req.glow_map) + " glow_map;" + term;
            if(req.has_bump_map())  block += "uniform " + sampler_type(req.bump_map) + " bump_map;" + term;
        }

        if(req.is_nolight()) {
            block += "uniform vec4 colour;" + term;
        }

        if(req.is_phong()) {
            block += build_light_block(req);
            block += build_material_block(req);
        }

        block += "out vec4 frag_colour;" + term;
        block += GLSL_OPEN_MAIN + term;

        if(req.is_phong()) block += build_light_computation(req);

        if(!req.has_textures()) block += "frag_colour = colour;" + term;
        else if(req.has_diffuse_map()) {
            if (req.diffuse_map == texture_type::TT_TEX2D) block += "frag_colour = texture(diffuse_map, tex2) * colour;" + term;
            else if (req.diffuse_map == texture_type::TT_CUBE_MAP) block += "frag_colour = texture(diffuse_map, tex3) * colour;" + term;
        }

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

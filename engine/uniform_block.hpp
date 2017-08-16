//
// Created by Darren Otgaar on 2016/05/15.
//

#ifndef ZAP_UNIFORM_BLOCK_HPP
#define ZAP_UNIFORM_BLOCK_HPP

#include <core/pod.hpp>

namespace zap { namespace engine {

// NOTE:  Prefer memory aligned on a 16-byte boundary.

#if defined(_WIN32)
#define ALIGN __declspec(align(16))
#else
#define ALIGN __attribute__((aligned(16)))
#endif

template <typename... Args>
#if defined(_WIN32)
ALIGN struct uniform_block : core::pod<Args...> {
#else
struct ALIGN uniform_block : core::pod<Args...> {
#endif
    using pod_t = core::pod<Args...>;
    constexpr static size_t size = sizeof...(Args);
    constexpr static size_t bytesize() { return sizeof(uniform_block); }
    using offsets = typename core::generate_table<size, pod_t, core::pod_offset>::result;

    uniform_block() = default;
    uniform_block(const Args&... args) : core::pod<Args...>(args...) { }
};

enum class ublock_attribute : size_t {
    UA_SCALE,                   // Common Variables
    UA_OFFSET,
    UA_VIEWPORT,                // Screen/Window Data [x, y, width, height]
    UA_CAM_RIGHT,
    UA_CAM_UP,
    UA_CAM_DIR,
    UA_EYE_POSITION,
    UA_EYE_DIR,
    UA_CAM_WORLD_TO_VIEW,
    UA_CAM_VIEW_TO_WORLD,
    UA_CAM_PROJECTION,
    UA_CAM_PROJ_VIEW,
    UA_LIGHTS,                  // General light arrays
    UA_LIGHTS_DIR,              // Directional light arrays
    UA_LIGHTS_POINT,            // Point light arrays
    UA_LIGHTS_SPOT,             // Spot light arrays
    UA_LIGHT_TYPE,              // 0 - dir, 1 - point, 2 - spot
    UA_LIGHT_COUNT,             // Number of lights in an array
    UA_LIGHT_POSITION,          // Point or spot light position
    UA_LIGHT_COLOUR,            // Basic light colour (* ADS)
    UA_LIGHT_DIR,               // Direction for spot light
    UA_LIGHT_AMBIENT,           // Ambient Light factor
    UA_LIGHT_DIFFUSE,           // Diffuse Light factor
    UA_LIGHT_SPECULAR,          // Specular Light factor
    UA_LIGHT_ATTENUATION,       // [Kc = constant, Kl = linear, Kq = quadratic]
    UA_LIGHT_INTENSITY,         // Basic intensity
    UA_LIGHT_ANGLE,             // Angle for spot light
    UA_LIGHT_COS_ANGLE,         // Cosine of Angle for spot light
    UA_LIGHT_SIN_ANGLE,         // Sine of Angle for spot light
    UA_LIGHT_EXPONENT,          // Dropoff exponent for spot light
    UA_LIGHT_TRANSFORM,         // Light Frenet frame
    UA_LIGHT_ADS,               // Light coefficients (ambient, diffuse, specular)
    UA_MATERIAL_EMISSIVE,       // Emissive Colour factor
    UA_MATERIAL_AMBIENT,        // Ambient material factor
    UA_MATERIAL_DIFFUSE,        // Diffuse material factor
    UA_MATERIAL_SPECULAR,       // Specular material factor
    UA_MATERIAL_EXPONENT,       // Specular exponent (or gloss/shininess)
    UA_PROJECTOR,
    UA_SCALE_BIAS,
    UA_MODEL_MATRIX,            // Transforms will bind here
    UA_MVP_MATRIX,
    UA_MV_MATRIX,
    UA_VIEW_MATRIX,
    UA_NORMAL_MATRIX,
    UA_USERDATA1,               // Use for special parameters, testing, etc.
    UA_USERDATA2
};

}

namespace core {
    MAKE_PODFIELD(scale, engine::ublock_attribute, engine::ublock_attribute::UA_SCALE)
    MAKE_PODFIELD(offset, engine::ublock_attribute, engine::ublock_attribute::UA_OFFSET)
    MAKE_PODFIELD(viewport, engine::ublock_attribute, engine::ublock_attribute::UA_VIEWPORT)
    MAKE_PODFIELD(cam_right, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_RIGHT)
    MAKE_PODFIELD(cam_up, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_UP)
    MAKE_PODFIELD(cam_dir, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_DIR)
    MAKE_PODFIELD(eye_position, engine::ublock_attribute, engine::ublock_attribute::UA_EYE_POSITION)
    MAKE_PODFIELD(eye_dir, engine::ublock_attribute, engine::ublock_attribute::UA_EYE_DIR)
    MAKE_PODFIELD(cam_world_to_view, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_WORLD_TO_VIEW)
    MAKE_PODFIELD(cam_view_to_world, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_VIEW_TO_WORLD)
    MAKE_PODFIELD(cam_projection, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_PROJECTION)
    MAKE_PODFIELD(cam_proj_view, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_PROJ_VIEW)
    MAKE_PODFIELD(lights, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHTS)
    MAKE_PODFIELD(lights_dir, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHTS_DIR)
    MAKE_PODFIELD(lights_point, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHTS_POINT)
    MAKE_PODFIELD(lights_spot, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHTS_SPOT)
    MAKE_PODFIELD(light_type, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_TYPE)
    MAKE_PODFIELD(light_count, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_COUNT)
    MAKE_PODFIELD(light_position, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_POSITION)
    MAKE_PODFIELD(light_colour, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_COLOUR)
    MAKE_PODFIELD(light_dir, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_DIR)
    MAKE_PODFIELD(light_ambient, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_AMBIENT)
    MAKE_PODFIELD(light_diffuse, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_DIFFUSE)
    MAKE_PODFIELD(light_specular, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_SPECULAR)
    MAKE_PODFIELD(light_attenuation, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_ATTENUATION)
    MAKE_PODFIELD(light_intensity, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_INTENSITY)
    MAKE_PODFIELD(light_angle, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_ANGLE)
    MAKE_PODFIELD(light_cos_angle, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_COS_ANGLE)
    MAKE_PODFIELD(light_sin_angle, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_SIN_ANGLE)
    MAKE_PODFIELD(light_exponent, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_EXPONENT)
    MAKE_PODFIELD(light_transform, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_TRANSFORM)
    MAKE_PODFIELD(light_ADS, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_ADS)
    MAKE_PODFIELD(material_emissive, engine::ublock_attribute, engine::ublock_attribute::UA_MATERIAL_EMISSIVE)
    MAKE_PODFIELD(material_ambient, engine::ublock_attribute, engine::ublock_attribute::UA_MATERIAL_AMBIENT)
    MAKE_PODFIELD(material_diffuse, engine::ublock_attribute, engine::ublock_attribute::UA_MATERIAL_DIFFUSE)
    MAKE_PODFIELD(material_specular, engine::ublock_attribute, engine::ublock_attribute::UA_MATERIAL_SPECULAR)
    MAKE_PODFIELD(material_exponent, engine::ublock_attribute, engine::ublock_attribute::UA_MATERIAL_EXPONENT)
    MAKE_PODFIELD(projector, engine::ublock_attribute, engine::ublock_attribute::UA_PROJECTOR)
    MAKE_PODFIELD(scale_bias, engine::ublock_attribute, engine::ublock_attribute::UA_SCALE_BIAS)
    MAKE_PODFIELD(model_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_MODEL_MATRIX)
    MAKE_PODFIELD(mvp_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_MVP_MATRIX)
    MAKE_PODFIELD(mv_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_MV_MATRIX)
    MAKE_PODFIELD(view_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_VIEW_MATRIX);
    MAKE_PODFIELD(normal_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_NORMAL_MATRIX)
    MAKE_PODFIELD(userdata1, engine::ublock_attribute, engine::ublock_attribute::UA_USERDATA1)
    MAKE_PODFIELD(userdata2, engine::ublock_attribute, engine::ublock_attribute::UA_USERDATA2)
}

}

#endif //ZAP_UNIFORM_BLOCK_HPP

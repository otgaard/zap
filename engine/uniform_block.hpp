//
// Created by Darren Otgaar on 2016/05/15.
//

#ifndef ZAP_UNIFORM_BLOCK_HPP
#define ZAP_UNIFORM_BLOCK_HPP

#include <core/pod.hpp>

namespace zap { namespace engine {

// NOTE:  Prefer memory aligned on a 16-byte boundary.

template <typename... Args>
struct uniform_block : core::pod<Args...> {
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
    UA_LIGHT_TYPE,              // 0 - dir, 1 - point, 2 - spot
    UA_LIGHT_EMISSIVE,
    UA_LIGHT_AMBIENT,
    UA_LIGHT_DIFFUSE,
    UA_LIGHT_SPECULAR,
    UA_LIGHT_ATTENUATION,       // [Kc, Kl, Kq, ?]
    UA_LIGHT_INTENSITY,
    UA_LIGHT_ANGLE,
    UA_LIGHT_COS_ANGLE,
    UA_LIGHT_SIN_ANGLE,
    UA_LIGHT_EXPONENT,
    UA_LIGHT_TRANSFORM,
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
    MAKE_PODFIELD(light_type, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_TYPE)
    MAKE_PODFIELD(light_emissive, engine::ublock_attribute, engine::ublock_attribute::UA_LIGHT_EMISSIVE)
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

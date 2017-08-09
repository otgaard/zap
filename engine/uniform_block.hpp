//
// Created by Darren Otgaar on 2016/05/15.
//

#ifndef ZAP_UNIFORM_BLOCK_HPP
#define ZAP_UNIFORM_BLOCK_HPP

#include <core/pod.hpp>

namespace zap { namespace engine {

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
    UA_CAM_POSITION,            // Camera will bind here
    UA_CAM_RIGHT,
    UA_CAM_UP,
    UA_CAM_DIR,
    UA_EYE_DIR,
    UA_CAM_WORLD_TO_VIEW,
    UA_CAM_PROJECTION,
    UA_CAM_PROJ_VIEW,
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
    MAKE_PODFIELD(cam_position, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_POSITION)
    MAKE_PODFIELD(cam_right, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_RIGHT)
    MAKE_PODFIELD(cam_up, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_UP)
    MAKE_PODFIELD(cam_dir, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_DIR)
    MAKE_PODFIELD(eye_dir, engine::ublock_attribute, engine::ublock_attribute::UA_EYE_DIR)
    MAKE_PODFIELD(cam_world_to_view, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_WORLD_TO_VIEW)
    MAKE_PODFIELD(cam_projection, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_PROJECTION)
    MAKE_PODFIELD(cam_proj_view, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_PROJ_VIEW)
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

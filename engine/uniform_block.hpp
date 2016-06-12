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

    uniform_block() { }
    uniform_block(Args... args) : core::pod<Args...>(args...) { }
};

enum class ublock_attribute : size_t {
    UA_SCALE,
    UA_CAM_POSITION,
    UA_CAM_RIGHT,
    UA_CAM_UP,
    UA_CAM_DIR,
    UA_CAM_VIEW,
    UA_CAM_PROJECTION,
    UA_MODEL_MATRIX,
    UA_MVP_MATRIX,
    UA_MV_MATRIX,
    UA_NORMAL_MATRIX
};

}

namespace core {
    MAKE_PODFIELD(scale, engine::ublock_attribute, engine::ublock_attribute::UA_SCALE);
    MAKE_PODFIELD(cam_position, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_POSITION);
    MAKE_PODFIELD(cam_right, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_RIGHT);
    MAKE_PODFIELD(cam_up, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_UP);
    MAKE_PODFIELD(cam_dir, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_DIR);
    MAKE_PODFIELD(cam_view, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_VIEW);
    MAKE_PODFIELD(cam_projection, engine::ublock_attribute, engine::ublock_attribute::UA_CAM_PROJECTION);
    MAKE_PODFIELD(model_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_MODEL_MATRIX);
    MAKE_PODFIELD(mvp_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_MVP_MATRIX);
    MAKE_PODFIELD(mv_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_MV_MATRIX);
    MAKE_PODFIELD(normal_matrix, engine::ublock_attribute, engine::ublock_attribute::UA_NORMAL_MATRIX);

}

}

#endif //ZAP_UNIFORM_BLOCK_HPP

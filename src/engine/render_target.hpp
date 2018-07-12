//
// Created by Darren Otgaar on 2018/07/12.
//

#ifndef ZAP_RENDER_TARGET_HPP
#define ZAP_RENDER_TARGET_HPP

#include "engine.hpp"
#include "pixel_format.hpp"
#include "pixel_buffer.hpp"

namespace zap { namespace engine {

enum class render_target_type {
    RT_RENDERBUFFER,
    RT_TEXTURE
};

class ZAPENGINE_EXPORT render_target {
public:
    explicit render_target(render_target_type rt_type) : rt_type_(rt_type) { }
    virtual ~render_target() = default;

    virtual bool allocate() = 0;
    virtual bool deallocate() = 0;
    bool is_allocated() const { return id_ != INVALID_RESOURCE; }

    bool is_renderbuffer() const { return rt_type_ == render_target_type::RT_RENDERBUFFER; }
    bool is_texture() const { return rt_type_ == render_target_type::RT_TEXTURE; }

    uint32_t resource_id() const { return id_; }
    pixel_format px_format() const { return pixel_fmt_; }
    pixel_datatype px_datatype() const { return pixel_dt_; }
    int32_t width() const { return w_; }
    int32_t height() const { return h_; }
    int32_t depth() const { return d_; }

protected:
    const render_target_type rt_type_;
    resource_t id_ = INVALID_IDX;
    pixel_format pixel_fmt_ = pixel_format::PF_NONE;
    pixel_datatype pixel_dt_ = pixel_datatype::PD_NONE;
    int32_t w_ = 0;
    int32_t h_ = 0;
    int32_t d_ = 0;
};

}}

#endif //ZAP_RENDER_TARGET_HPP

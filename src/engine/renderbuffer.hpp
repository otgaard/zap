//
// Created by Darren Otgaar on 2018/07/12.
//

#ifndef ZAP_RENDERBUFFER_HPP
#define ZAP_RENDERBUFFER_HPP

#include "render_target.hpp"

namespace zap { namespace engine {

class ZAPENGINE_EXPORT renderbuffer : public render_target {
public:
    static constexpr auto rt_type = render_target_type::RT_RENDERBUFFER;

    renderbuffer() : render_target(rt_type) { }
    renderbuffer(pixel_format pf, pixel_datatype dt, int32_t width, int32_t height) : render_target(rt_type) {
        pixel_fmt_ = pf;
        pixel_dt_ = dt;
        w_ = width;
        h_ = height;
        d_ = 1;
        int_fmt_ = gl_internal_format(pf, dt);
    }

    virtual ~renderbuffer() { if(is_allocated()) deallocate(); }

    bool allocate() override;
    bool deallocate() override;
    bool initialise();
    bool initialise(pixel_format pf, pixel_datatype dt, int32_t width, int32_t height);

    void bind();
    void release();

private:
    uint32_t int_fmt_;
};

}}

#endif //ZAP_RENDERBUFFER_HPP

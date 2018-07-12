//
// Created by Darren Otgaar on 2018/07/12.
//

#ifndef ZAP_RENDERBUFFER_HPP
#define ZAP_RENDERBUFFER_HPP

#include "engine.hpp"
#include "pixel_format.hpp"
#include "pixel_buffer.hpp"

namespace zap { namespace engine {

class ZAPENGINE_EXPORT renderbuffer {
public:
    renderbuffer();
    renderbuffer(pixel_format pf, pixel_datatype dt, int32_t width, int32_t height);
    ~renderbuffer();

    bool allocate();
    bool deallocate();
    bool is_allocated() const { return id_ != INVALID_RESOURCE; }

    bool initialise();

private:
    resource_t id_ = INVALID_IDX;
    pixel_format pixel_fmt_;
    pixel_datatype datatype_;
    int32_t w_ = 0;
    int32_t h_ = 0;
};


}}

#endif //ZAP_RENDERBUFFER_HPP

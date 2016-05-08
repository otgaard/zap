/* Created by Darren Otgaar on 2016/04/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_PIXEL_BUFFER_HPP
#define ZAP_PIXEL_BUFFER_HPP

#include "buffer.hpp"
#include "pixel_format.hpp"

namespace zap { namespace engine {
    template <typename PIX_T, buffer_usage USAGE>
    class pixel_buffer : public buffer {
    public:
        using pixel_t = PIX_T;
    };
}}

#endif //ZAP_PIXEL_BUFFER_HPP

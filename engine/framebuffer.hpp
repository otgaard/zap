/* Created by Darren Otgaar on 2016/05/27. http://www.github.com/otgaard/zap */
#ifndef ZAP_FRAMEBUFFER_HPP
#define ZAP_FRAMEBUFFER_HPP

#include "engine.hpp"
#include "texture.hpp"

namespace zap { namespace engine {

class framebuffer {
public:
    template <typename PixelT>
    framebuffer(size_t target_count, size_t width, size_t height, bool mipmaps, bool depthstencil);
    ~framebuffer() { if(is_allocated()) deallocate(); }

    bool allocate();
    void deallocate();
    bool is_allocated() const { return framebuffer_ != INVALID_RESOURCE; }

    void bind() const;
    void release() const;
    bool is_bound() const;

    bool initialise();
    bool is_initialised() const { return attachments_.size() > 0; }

protected:
    resource_t framebuffer_;
    size_t target_count_;
    size_t width_;
    size_t height_;
    bool mipmaps_;
    bool depthstencil_;
    pixel_format pix_format_;
    pixel_datatype pix_dtype_;
    std::vector<texture> attachments_;  // If depthstencil == true, attachments_.size() == target_count + 1
};

template <typename PixelT>
framebuffer::framebuffer(size_t target_count, size_t width, size_t height, bool mipmaps, bool depthstencil) :
        target_count_(target_count), width_(width), height_(height), mipmaps_(mipmaps), depthstencil_(depthstencil),
        pix_format_(pixel_type<PixelT>::format), pix_dtype_(pixel_type<PixelT>::datatype) {
}

}}

#endif //ZAP_FRAMEBUFFER_HPP

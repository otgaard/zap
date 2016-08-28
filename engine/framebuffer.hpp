/* Created by Darren Otgaar on 2016/05/27. http://www.github.com/otgaard/zap */
#ifndef ZAP_FRAMEBUFFER_HPP
#define ZAP_FRAMEBUFFER_HPP

#include "engine.hpp"
#include "texture.hpp"

namespace zap { namespace engine {

class framebuffer {
public:
    framebuffer() : framebuffer_(INVALID_IDX), target_count_(0) { }
    framebuffer(size_t target_count, size_t width, size_t height, pixel_format format, pixel_datatype datatype,
                bool mipmaps, bool depthstencil);
    ~framebuffer() { if(is_allocated()) deallocate(); }

    inline size_t width() const { return width_; }
    inline size_t height() const { return height_; }

    bool allocate();
    void deallocate();
    bool is_allocated() const { return framebuffer_ != INVALID_RESOURCE; }

    void bind() const;
    void release() const;
    bool is_bound() const;

    const texture& get_attachment(size_t idx) const { checkidx(idx, target_count_); return attachments_[idx]; }

    bool initialise();
    bool initialise(size_t target_count, size_t width, size_t height, pixel_format format, pixel_datatype datatype,
        bool mipmaps, bool depthstencil);
    template <typename PixelT>
    bool initialise(size_t target_count, size_t width, size_t height, bool mipmaps, bool depthstencil) {
        return initialise(target_count, width, height, pixel_type<PixelT>::format, pixel_type<PixelT>::datatype,
            mipmaps, depthstencil);
    }
    bool is_initialised() const { return attachments_.size() > 0; }

protected:
    resource_t framebuffer_;
    size_t target_count_;
    size_t width_;
    size_t height_;
    pixel_format pix_format_;
    pixel_datatype pix_dtype_;
    std::vector<texture> attachments_;  // If depthstencil == true, attachments_.size() == target_count + 1
    std::vector<uint32_t> draw_buffers_;
    bool mipmaps_;
    bool depthstencil_;
    mutable int32_t curr_viewport_[4];
    mutable double curr_depthrange_[2];
};

inline framebuffer::framebuffer(size_t target_count, size_t width, size_t height, pixel_format format, pixel_datatype datatype,
        bool mipmaps, bool depthstencil) : target_count_(target_count), width_(width), height_(height), pix_format_(format),
        pix_dtype_(datatype), mipmaps_(mipmaps), depthstencil_(depthstencil) {
}


}}

#endif //ZAP_FRAMEBUFFER_HPP

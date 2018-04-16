/* Created by Darren Otgaar on 2016/05/27. http://www.github.com/otgaard/zap */
#ifndef ZAP_FRAMEBUFFER_HPP
#define ZAP_FRAMEBUFFER_HPP

#include "engine.hpp"
#include "texture.hpp"
#include <maths/vec4.hpp>

namespace zap { namespace engine {

class ZAPENGINE_EXPORT framebuffer {
public:
    using vec4i = maths::vec4i;

    framebuffer() = default;
    framebuffer(const framebuffer&) = delete;
    framebuffer(framebuffer&&) = delete;
    framebuffer(size_t target_count, size_t width, size_t height, pixel_format format, pixel_datatype datatype,
                bool mipmaps, bool depthstencil);
    ~framebuffer() { if(is_allocated()) deallocate(); }

    framebuffer& operator=(const framebuffer&) = delete;
    framebuffer& operator=(framebuffer&&) = delete;

    inline size_t width() const { return width_; }
    inline size_t height() const { return height_; }

    bool allocate();
    void deallocate();
    bool is_allocated() const { return framebuffer_ != INVALID_RESOURCE; }

    void bind() const;
    void release() const;
    bool is_bound() const;

    const texture& get_attachment(size_t idx) const {
        checkidx(idx, target_count_ + depthstencil_);
        return attachments_[idx];
    }

    // Framebuffer must be bound for read/write operations to succeed
    // Note: viewport = [x, y, width, height]
    template <typename PixelT>
    bool read_attachment(pixel_buffer<PixelT>& pbuf, const vec4i& viewport, size_t idx) const {
        checkidx(idx, target_count_ + depthstencil_);

        // Check type is compatible
        if(pixel_type<PixelT>::format != pix_format_ || pixel_type<PixelT>::datatype != pix_dtype_) {
            LOG_ERR("Cannot copy framebuffer attachment with mismatched pixel format to data type");
            return false;
        }

        pbuf.bind(pbuf.read_type);
        auto err = read_attachment(viewport, idx);
        pbuf.release(pbuf.read_type);
        return err;
    }

    bool read_attachment(buffer& buf, const vec4i& viewport, size_t idx) const {
        checkidx(idx, target_count_ + depthstencil_);
        buf.bind(buffer_type::BT_PIXEL_PACK);
        auto err = read_attachment(viewport, idx);
        buf.release(buffer_type::BT_PIXEL_PACK);
        return err;
    }

    template <typename PixelT>
    bool write_attachment(const pixel_buffer<PixelT>& pbuf, const vec4i& viewport, size_t idx) {
        checkidx(idx, target_count_ + depthstencil_);

        // Check type is compatible
        if(pixel_type<PixelT>::format != pix_format_ || pixel_type<PixelT>::datatype != pix_dtype_) {
            LOG_ERR("Cannot copy framebuffer attachment with mismatched pixel format to data type");
            return false;
        }

        pbuf.bind(pbuf.write_type);
        auto err = attachments_[idx].copy(pbuf, viewport[0], viewport[1], viewport[2], viewport[3], 0, false);
        pbuf.release(pbuf.write_type);
        return err;
    }

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
    // viewport = [x, y, width, height]
    bool read_attachment(const vec4i& viewport, size_t idx) const;

    resource_t framebuffer_ = INVALID_IDX;
    size_t target_count_ = 0;
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
                                bool mipmaps, bool depthstencil) :
                                      target_count_(target_count), width_(width),
                                      height_(height), pix_format_(format),
                                      pix_dtype_(datatype), mipmaps_(mipmaps),
                                      depthstencil_(depthstencil) {
}


}}

#endif //ZAP_FRAMEBUFFER_HPP

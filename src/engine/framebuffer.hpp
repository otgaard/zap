/* Created by Darren Otgaar on 2016/05/27. http://www.github.com/otgaard/zap */
#ifndef ZAP_FRAMEBUFFER_HPP
#define ZAP_FRAMEBUFFER_HPP

#include "engine.hpp"
#include "texture.hpp"
#include "renderbuffer.hpp"
#include <maths/vec4.hpp>

namespace zap { namespace engine {

enum class framebuffer_type {
    FT_COLOUR = 1 << 0,
    FT_DEPTH = 1 << 1,
    FT_STENCIL = 1 << 2,
    FT_COLOUR_DEPTH = FT_COLOUR | FT_DEPTH,
    FT_COLOUR_STENCIL = FT_COLOUR | FT_STENCIL,
    FT_COLOUR_DEPTH_STENCIL = FT_COLOUR | FT_DEPTH | FT_STENCIL,
    FT_DEPTH_STENCIL = FT_DEPTH | FT_STENCIL,
};

enum class attachment_type {
    AT_NONE,
    AT_TEXTUREBUFFER,
    AT_RENDERBUFFER
};

class ZAPENGINE_EXPORT framebuffer {
public:
    using vec4i = maths::vec4i;

    framebuffer() = default;
    framebuffer(framebuffer_type ft);
    framebuffer(framebuffer_type ft, attachment_type colour_type, attachment_type depth_type=attachment_type::AT_NONE);
    framebuffer(const framebuffer&) = delete;
    framebuffer(framebuffer&&) = delete;
    /*
    framebuffer(size_t target_count, size_t width, size_t height, pixel_format format, pixel_datatype datatype,
                bool mipmaps, bool depthstencil);
    */
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

    void set_framebuffer_type(framebuffer_type ft);
    void set_colour_target(attachment_type at, pixel_format pf, pixel_datatype pd);
    void set_depth_stencil_target(attachment_type at, pixel_format pf, pixel_datatype pd);
    bool is_valid_format() const;

    bool has_attachment(framebuffer_type ft) const { return (uint32_t(ftype_) & uint32_t(ft)) != 0; }
    bool has_colour() const { return has_attachment(framebuffer_type::FT_COLOUR); }
    bool has_depth() const { return has_attachment(framebuffer_type::FT_DEPTH); }
    bool has_stencil() const { return has_attachment(framebuffer_type::FT_STENCIL); }
    bool has_depth_stencil() const {
        return has_attachment(framebuffer_type(
                uint32_t(framebuffer_type::FT_STENCIL)
                | uint32_t(framebuffer_type::FT_DEPTH)));
    }
    framebuffer_type get_framebuffer_type() const { return ftype_; }
    attachment_type get_colour_attachment() const { return col_attachment_; }
    attachment_type get_depth_stencil_attachment() const { return ds_attachment_; }

    size_t get_attachment_count() const {
        return (get_colour_attachment() == attachment_type::AT_TEXTUREBUFFER ? colour_target_count_ : 0)
               + (get_depth_stencil_attachment() == attachment_type::AT_TEXTUREBUFFER ? 1 : 0);
    }

    const texture& get_attachment(size_t idx) const {
        checkidx(idx, colour_target_count_ + has_depth_stencil());
        auto rt = attachments_[idx].get();
        return *(rt->is_texture() ? dynamic_cast<texture*>(rt) : nullptr);
    }

    // Framebuffer must be bound for read/write operations to succeed
    // Note: viewport = [x, y, width, height]
    template <typename PixelT>
    bool read_attachment(pixel_buffer<PixelT>& pbuf, const vec4i& viewport, size_t idx) const {
        checkidx(idx, colour_target_count_ + has_depth_stencil());

        const auto& fmt = idx == colour_target_count_ ? pf_ds_ : pf_col_;
        const auto& dt = idx == colour_target_count_ ? dt_ds_ : dt_col_;

        // Check type is compatible
        if(pixel_type<PixelT>::format != fmt || pixel_type<PixelT>::datatype != dt) {
            LOG_ERR("Cannot copy framebuffer attachment with mismatched pixel format to data type");
            return false;
        }

        pbuf.bind(pbuf.read_type);
        auto err = read_attachment(viewport, idx);
        pbuf.release(pbuf.read_type);
        return err;
    }

    bool read_attachment(buffer& buf, const vec4i& viewport, size_t idx) const {
        checkidx(idx, colour_target_count_ + has_depth_stencil());
        buf.bind(buffer_type::BT_PIXEL_PACK);
        auto err = read_attachment(viewport, idx);
        buf.release(buffer_type::BT_PIXEL_PACK);
        return err;
    }

    template <typename PixelT>
    bool write_attachment(const pixel_buffer<PixelT>& pbuf, const vec4i& viewport, size_t idx) {
        checkidx(idx, colour_target_count_ + has_depth_stencil());

        const auto& fmt = idx == colour_target_count_ ? pf_ds_ : pf_col_;
        const auto& dt = idx == colour_target_count_ ? dt_ds_ : dt_col_;

        if(pixel_type<PixelT>::format != fmt || pixel_type<PixelT>::datatype != dt) {
            LOG_ERR("Cannot copy framebuffer attachment with mismatched pixel format to data type");
            return false;
        }

        pbuf.bind(pbuf.write_type);
        auto rt = attachments_[idx]->is_texture() ? dynamic_cast<texture*>(attachments_[idx].get()) : nullptr;
        if(!rt) return false;

        auto err = rt->copy(pbuf, viewport[0], viewport[1], viewport[2], viewport[3], 0, false);
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

    framebuffer_type ftype_;
    size_t width_;
    size_t height_;
    bool mipmaps_;

    attachment_type col_attachment_;
    pixel_format pf_col_;
    pixel_datatype dt_col_;
    attachment_type ds_attachment_;
    pixel_format pf_ds_;
    pixel_datatype dt_ds_;

    resource_t framebuffer_ = INVALID_IDX;
    size_t colour_target_count_ = 0;
    std::vector<std::unique_ptr<render_target>> attachments_;

    mutable std::array<int32_t, 4> curr_viewport_;
    mutable std::array<double, 2> curr_depthrange_;
};

}}

#endif //ZAP_FRAMEBUFFER_HPP

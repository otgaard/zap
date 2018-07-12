/* Created by Darren Otgaar on 2016/05/07. http://www.github.com/otgaard/zap */
#ifndef ZAP_TEXTURE_HPP
#define ZAP_TEXTURE_HPP

#include <vector>
#include "render_target.hpp"

namespace zap { namespace engine {

class ZAPENGINE_EXPORT texture : public render_target {
public:
    static constexpr auto rt_type = render_target_type::RT_TEXTURE;

    explicit texture(texture_type type=texture_type::TT_TEX2D) : render_target(rt_type), type_(type) { }
    virtual ~texture() { if(is_allocated()) deallocate(); }
    texture(const texture& rhs) = delete;
    texture(texture&& rhs) noexcept : render_target(rt_type), type_(rhs.type_) {
        id_ = rhs.id_;
        w_ = rhs.w_;
        h_ = rhs.h_;
        d_ = rhs.d_;
        rhs.id_ = INVALID_RESOURCE;
    }

    texture& operator=(const texture& rhs) = delete;
    texture& operator=(texture&& rhs) noexcept {
        if(this != &rhs) {
            std::swap(type_, rhs.type_);
            std::swap(id_, rhs.id_);
            w_ = rhs.w_; h_ = rhs.h_; d_ = rhs.d_;
        }
        return *this;
    }

    void set_type(texture_type type) { if(!is_allocated()) type_ = type; }
    texture_type get_type() const { return type_; }
    void set_wrap_s(tex_wrap mode) { if(!is_allocated()) wrap_mode_[0] = mode; }
    void set_wrap_t(tex_wrap mode) { if(!is_allocated()) wrap_mode_[1] = mode; }
    void set_wrap_r(tex_wrap mode) { if(!is_allocated()) wrap_mode_[2] = mode; }
    tex_wrap get_wrap_s() const { return wrap_mode_[0]; }
    tex_wrap get_wrap_t() const { return wrap_mode_[1]; }
    tex_wrap get_wrap_r() const { return wrap_mode_[2]; }
    void set_min_filter(tex_filter filter) { if(!is_allocated()) min_filter_ = filter; }
    void set_mag_filter(tex_filter filter) { if(!is_allocated()) mag_filter_ = filter; }
    tex_filter get_min_filter() const { return min_filter_; }
    tex_filter get_mag_filter() const { return mag_filter_; }

    bool allocate();
    bool deallocate();

    resource_t resource() const { return id_; }

    void bind(size_t unit=0) const;
    void release() const;
    bool is_bound() const;

    bool copy(size_t col, size_t row, size_t width, size_t height, int level, bool update_mipmaps, pixel_format format,
        pixel_datatype datatype, const char* data=nullptr);

    bool initialise(texture_type type, int width, int height, int depth, pixel_format format,
                    pixel_datatype datatype, bool mipmaps, const char* data=nullptr);


    template <typename PixelT>
    bool initialise(size_t width, size_t height, const std::vector<PixelT>& buffer, bool generate_mipmaps=false) {
        return initialise(texture_type::TT_TEX2D, uint32_t(width), uint32_t(height), 1, pixel_type<PixelT>::format,
                          pixel_type<PixelT>::datatype, generate_mipmaps, reinterpret_cast<const char*>(buffer.data()));
    }

    template <typename PixelT>
    bool initialise(const pixel_buffer<PixelT>& pixbuf, bool generate_mipmaps=false) {
        texture_type type;
        if(pixbuf.depth() == 6)      type = texture_type::TT_CUBE_MAP;
        else if(pixbuf.depth() > 1)  type = texture_type::TT_TEX3D;
        else if(pixbuf.height() > 1) type = texture_type::TT_TEX2D;
        else                         type = texture_type::TT_TEX1D;
        pixbuf.bind();
        auto err = initialise(type, int32_t(pixbuf.width()), int32_t(pixbuf.height()), int32_t(pixbuf.depth()),
                              pixel_type<PixelT>::format, pixel_type<PixelT>::datatype, generate_mipmaps, nullptr);
        pixbuf.release();
        release();
        return err;
    }

    template <typename PixelT>
    bool initialise(const pixmap<PixelT>& pmap, bool generate_mipmaps=false) {
        texture_type type;
        if(pmap.depth() == 6)      type = texture_type::TT_CUBE_MAP;
        else if(pmap.depth() > 1)  type = texture_type::TT_TEX3D;
        else if(pmap.height() > 1) type = texture_type::TT_TEX2D;
        else                       type = texture_type::TT_TEX1D;
        return initialise(type, int32_t(pmap.width()), int32_t(pmap.height()), int32_t(pmap.depth()),
                          pixel_type<PixelT>::format, pixel_type<PixelT>::datatype, generate_mipmaps,
                          reinterpret_cast<const char*>(pmap.data()));
    }

    template <typename PixelT>
    bool copy(const pixel_buffer<PixelT>& pixbuf, size_t col, size_t row, size_t width, size_t height,
                int level=0, bool update_mipmaps=false) {
        pixbuf.bind();
        auto err = copy(col, row, width, height, level, update_mipmaps, pixel_type<PixelT>::format, pixel_type<PixelT>::datatype, 0);
        pixbuf.release();
        return err;
    };

    static size_t query_max_units();

protected:
    void initialise_default();

    texture_type type_ = texture_type::TT_TEX2D;
    tex_wrap wrap_mode_[3] = { tex_wrap::TW_CLAMP_TO_EDGE, tex_wrap::TW_CLAMP_TO_EDGE, tex_wrap::TW_CLAMP_TO_EDGE };
    tex_filter min_filter_ = tex_filter::TF_NEAREST, mag_filter_ = tex_filter::TF_NEAREST;
};

}}

#endif //ZAP_TEXTURE_HPP

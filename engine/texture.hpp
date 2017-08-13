/* Created by Darren Otgaar on 2016/05/07. http://www.github.com/otgaard/zap */
#ifndef ZAP_TEXTURE_HPP
#define ZAP_TEXTURE_HPP

#include <vector>
#include "engine.hpp"
#include "pixel_format.hpp"
#include "pixel_buffer.hpp"

namespace zap { namespace engine {

class texture {
public:
    explicit texture(texture_type type=texture_type::TT_TEX2D) : type_(type) { }
    ~texture() { if(is_allocated()) deallocate(); }
    texture(const texture& rhs) = delete;
    texture(texture&& rhs) noexcept : type_(rhs.type_), id_(rhs.id_), w_(rhs.w_), h_(rhs.h_), d_(rhs.d_) {
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

    inline int width() const { return w_; }
    inline int height() const { return h_; }
    inline int depth() const { return d_; }

    bool allocate();
    bool deallocate();
    bool is_allocated() const { return id_ != INVALID_RESOURCE; }

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
        return initialise(texture_type::TT_TEX2D, width, height, 1, pixel_type<PixelT>::format,
                          pixel_type<PixelT>::datatype, generate_mipmaps, reinterpret_cast<const char*>(buffer.data()));
    }

    template <typename PixelT>
    bool initialise(const pixel_buffer<PixelT>& pixbuf, bool generate_mipmaps=false) {
        texture_type type;
        if(pixbuf.depth() > 1) type = texture_type::TT_TEX3D;
        else if(pixbuf.height() > 1) type = texture_type::TT_TEX2D;
        else type = texture_type::TT_TEX1D;
        pixbuf.bind();
        auto err = initialise(type, pixbuf.width(), pixbuf.height(), pixbuf.depth(), pixel_type<PixelT>::format,
            pixel_type<PixelT>::datatype, generate_mipmaps, nullptr);
        pixbuf.release();
        release();
        return err;
    }

    template <typename PixelT>
    bool initialise(const pixmap<PixelT>& pmap, bool generate_mipmaps=false) {
        texture_type type;
        if(pmap.depth() > 1) type = texture_type::TT_TEX3D;
        else if(pmap.height() > 1) type = texture_type::TT_TEX2D;
        else type = texture_type::TT_TEX1D;
        return initialise(type, pmap.width(), pmap.height(), pmap.depth(), pixel_type<PixelT>::format,
                          pixel_type<PixelT>::datatype, generate_mipmaps, reinterpret_cast<const char*>(pmap.data()));
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
    resource_t id_ = INVALID_IDX;
    int w_ = 0;
    int h_ = 0;
    int d_ = 0;
};

}}

#endif //ZAP_TEXTURE_HPP

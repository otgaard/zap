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

    // Test Function
    template <typename Pixel>
    bool initialise(size_t width, size_t height, const std::vector<Pixel>& buffer, bool generate_mipmaps=false);
    bool initialise(texture_type type, size_t width, size_t height, pixel_format format, pixel_datatype datatype,
                    const void* data=nullptr);
    template <typename PixelT>
    bool initialise(const pixel_buffer<PixelT>& pixbuf, bool generate_mipmaps=false);
    template <typename PixelT>
    bool initialise(const pixmap<PixelT>& pmap, bool generate_mipmaps=false);

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
    texture_type type_ = texture_type::TT_TEX2D;
    resource_t id_ = 0;
    int w_ = 0;
    int h_ = 0;
    int d_ = 0;
};

}}

#endif //ZAP_TEXTURE_HPP

/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */
#ifndef ZAP_PIXMAP_HPP
#define ZAP_PIXMAP_HPP

#include "pixel_format.hpp"
#include <maths/rect.hpp>
#include <vector>

namespace zap { namespace engine {
    template <typename PixelT>
    class pixmap {
    public:
        using buffer_t = std::vector<PixelT>;
        pixmap() : width_(0), height_(0), depth_(0) { }
        pixmap(int width, int height=1, int depth=1) { resize(width, height, depth); }
        ~pixmap() = default;

        void resize(int width, int height=1, int depth=1) {
            if(width < 1 || height < 1 || depth < 1) return;
            width_ = width; height_ = height; depth_ = depth;
            buffer_.resize(size_t(width*height*depth));
        }

        inline int width() const { return width_; }
        inline int height() const { return height_; }
        inline int depth() const { return depth_; }
        inline int size() const { return int(buffer_.size()); }
        inline size_t bytesize() const { return buffer_.size()*sizeof(PixelT); }

        inline bool is_initialised() const { return width_ != 0; }
        inline bool is_1D() const { return height_ == 1 && depth_ == 1; }
        inline bool is_2D() const { return height_ > 1 && depth_ == 1; }
        inline bool is_3D() const { return height_ > 1 && depth_ == 1; }

        PixelT& operator[](int idx) { return buffer_[idx]; }
        const PixelT& operator[](int idx) const { return buffer_[idx]; }
        PixelT& operator()(int c) { return buffer_[c]; }
        const PixelT& operator()(int c) const { return buffer_[c]; }
        PixelT& operator()(int c, int r) { return buffer_[r*width_ + c]; }
        const PixelT& operator()(int c, int r) const { return buffer_[r*width_ + c]; }
        PixelT& operator()(int c, int r, int d) { return buffer_[width_*(d*height_ + r) + c]; }
        const PixelT& operator()(int c, int r, int d) const { return buffer_[width_*(d*height_ + r) + c]; }

        int offset(int c, int r, int d=0) const { return width_*(d*height_ + r) + c; }

        const PixelT* data(size_t offset=0) const { return buffer_.data()+offset; }
        const buffer_t& buffer() const { return buffer_; }

    protected:

    private:
        int width_, height_, depth_;
        buffer_t buffer_;
    };
}}

#endif //ZAP_PIXMAP_HPP

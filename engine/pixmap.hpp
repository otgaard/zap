/* Created by Darren Otgaar on 2016/08/20. http://www.github.com/otgaard/zap */
#ifndef ZAP_PIXMAP_HPP
#define ZAP_PIXMAP_HPP

#include "pixel_format.hpp"
#include <maths/geometry/rect.hpp>
#include <vector>

namespace zap { namespace engine {
    template <typename PixelT>
    class pixmap {
    public:
        using vec2i = maths::vec2i;
        using buffer_t = std::vector<PixelT>;
        pixmap() : width_(0), height_(0), depth_(0) { }
        pixmap(int width, int height=1, int depth=1) { resize(width, height, depth); }
        pixmap(int width, int height, int depth, const std::vector<PixelT>& buffer) :
            width_(width), height_(height), depth_(depth), buffer_(buffer) { }
        pixmap(const pixmap&) = default;
        ~pixmap() = default;

        pixmap& operator=(const pixmap&) = default;

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
        inline bool is_3D() const { return height_ > 1 && depth_ > 1; }

        PixelT& operator[](int idx) { return buffer_[idx]; }
        const PixelT& operator[](int idx) const { return buffer_[idx]; }
        PixelT& operator()(int c) { return buffer_[c]; }
        const PixelT& operator()(int c) const { return buffer_[c]; }
        PixelT& operator()(int c, int r) { return buffer_[r*width_ + c]; }
        const PixelT& operator()(int c, int r) const { return buffer_[r*width_ + c]; }
        PixelT& operator()(int c, int r, int d) { return buffer_[width_*(d*height_ + r) + c]; }
        const PixelT& operator()(int c, int r, int d) const { return buffer_[width_*(d*height_ + r) + c]; }

        int offset(int c, int r) const { return width_*r + c; }
        int offset(int c, int r, int d) const { return width_*(d*height_ + r) + c; }
        vec2i coord2(int idx) const { return vec2i(idx / width_, idx % width_); }

        template <typename Fnc>
        void blend(int origin_x, int origin_y, const pixmap& src, Fnc&& fnc) {
            assert(src.is_2D() && is_2D() && "pixmap::blend 2D only");
            origin_x = std::max(origin_x, 0); origin_y = std::max(origin_y, 0);
            auto end_x = std::min(origin_x+src.width(), width()), end_y = std::min(origin_y+src.height(), height());

            auto& ref = *this;
            for(auto r = origin_y; r != end_y; ++r) {
                for(auto c = origin_x; c != end_x; ++c) {
                    ref(c, r) = fnc(ref(c, r), src(c - origin_x, r - origin_y));
                }
            }
        }

        template <typename Fnc>
        void transform(int origin_x, int origin_y, int w, int h, Fnc&& fnc) {
            assert(is_2D() && "pixmap::transform 2D only");
            origin_x = std::max(origin_x, 0); origin_y = std::max(origin_y, 0);
            auto end_x = std::min(origin_x+w, width()), end_y = std::min(origin_y+h, height());
            auto& ref = *this;
            for(auto r = origin_y; r != end_y; ++r) {
                for(auto c = origin_x; c != end_x; ++c) {
                    ref(c, r) = fnc(ref(c, r));
                }
            }
        }

        void flip_y() {
            assert(is_2D() && "pixmap::flip_y 2D only");
            if(!is_2D()) return;

            auto& ref = *this;
            auto hm1 = height_ - 1;
            for(int r = 0, r_end = height_/2; r != r_end; ++r) {
                for(int c = 0; c != width_; ++c) {
                    std::swap(ref(c, hm1 - r), ref(c, r));
                }
            }
        }

        const PixelT* begin() const { return buffer_.data(); }
        const PixelT* end() const { return buffer_.data()+(width_*height_*depth_); }

        bool copy(const byte* data, size_t len) {
            assert(size()*sizeof(PixelT) >= len && "pixmap::copy out-of-bounds");
            if(size()*sizeof(PixelT) < len) return false;
            std::copy(data, data+len, reinterpret_cast<byte*>(buffer_.data()));
            return true;
        }

        size_t copy(const size_t src_off, const size_t trg_off, size_t length) {
            std::copy(data()+src_off, data()+src_off+length, data()+trg_off);
            return length;
        }

        const PixelT* data(size_t offset=0) const { return buffer_.data()+offset; }
        PixelT* data(size_t offset=0) { return buffer_.data()+offset; }
        const PixelT* data(int c, int r) const { return buffer_.data()+offset(c,r); }
        PixelT* data(int c, int r) { return buffer_.data()+offset(c,r); }
        const PixelT* data(int c, int r, int d) const { return buffer_.data()+offset(c,r,d); }
        PixelT* data(int c, int r, int d) { return buffer_.data()+offset(c,r,d); }

        const buffer_t& buffer() const { return buffer_; }

    protected:

    private:
        int width_ = 0, height_ = 0, depth_ = 0;
        buffer_t buffer_;
    };
}}

#endif //ZAP_PIXMAP_HPP

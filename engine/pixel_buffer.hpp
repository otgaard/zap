/* Created by Darren Otgaar on 2016/04/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_PIXEL_BUFFER_HPP
#define ZAP_PIXEL_BUFFER_HPP

#include "buffer.hpp"
#include "pixmap.hpp"
#include "pixel_format.hpp"

namespace zap { namespace engine {
    template <typename PIX_T, buffer_usage USAGE>
    class pixel_buffer : public buffer {
    public:
        using pixel_t = PIX_T;
        using pixmap_t = pixmap<pixel_t>;
        constexpr static auto read_type = buffer_type::BT_PIXEL_PACK;
        constexpr static auto write_type = buffer_type::BT_PIXEL_UNPACK;
        constexpr static auto usage = USAGE;

        pixel_buffer() : pixel_count_(0), width_(0), height_(0), depth_(0), write_bound_(false) { }

        inline size_t idx(size_t col, size_t row) const { return row*width_ + col; }
        inline size_t idx(size_t col, size_t row, size_t dep) const { return width_*(row + dep*height_) + col; }

        inline size_t width() const { return width_; }
        inline size_t height() const { return height_; }
        inline size_t depth() const { return depth_; }
        inline size_t pixel_count() const { return pixel_count_; }

        void bind(bool write) const { buffer::bind(write ? write_type : read_type); write_bound_ = write; }
        void release() const { buffer::release(write_bound_ ? write_type : read_type); write_bound_ = false; }

        bool initialise(size_t width, size_t height, const pixel_t* data=nullptr) {
            if(buffer::initialise(write_type, usage, width*height*pixel_t::bytesize, reinterpret_cast<const char*>(data))) {
                pixel_count_ = width*height; width_ = width; height_ = height; depth_ = 1;
                return true;
            }
            return false;
        }

        bool resize(size_t width, size_t height) {
            if(buffer::initialise(write_type, usage, width*height*pixel_t::bytesize)) {
                pixel_count_ = width*height; width_ = width; height_ = height; depth_ = 1;
                return true;
            }
            return false;
        }

        char* map(buffer_access access, bool write) { return buffer::map(write ? write_type : read_type, access); }
        char* map(buffer_access access, bool write, size_t offset, size_t length) {
            return buffer::map(write ? write_type : read_type, access, offset, length);
        }
        bool unmap(bool write) { return buffer::unmap(write ? write_type: read_type); }

        size_t copy(const typename pixmap_t::buffer_t& buffer, size_t src_off, size_t trg_off, size_t pixel_count) {
            const size_t length = pixel_count * pixel_t::bytesize;
            const size_t src_offset = src_off * pixel_t::bytesize;
            const size_t trg_offset = trg_off * pixel_t::bytesize;
            if(src_offset + length <= buffer.size()*pixel_t::bytesize && trg_offset + length <= size_) {
                bind(true);
                buffer::copy(write_type, trg_offset, length, reinterpret_cast<const char*>(buffer.data())+src_offset);
                release();
                return pixel_count;
            }
            return 0;
        }

        const pixel_t& operator[](size_t idx) const {
            assert(is_mapped() && "Pixel Buffer must be mapped!");
            assert(idx < pixel_count_ && ZERR_IDX_OUT_OF_RANGE);
            return *(reinterpret_cast<pixel_t*>(mapped_ptr_) + idx);
        }

        pixel_t& operator[](size_t idx) {
            assert(is_mapped() && "Pixel Buffer must be mapped!");
            assert(idx < pixel_count_ && ZERR_IDX_OUT_OF_RANGE);
            return *(reinterpret_cast<pixel_t*>(mapped_ptr_) + idx);
        }

        const pixel_t& operator()(size_t col, size_t row) const {
            assert(is_mapped() && "Pixel Buffer must be mapped!");
            assert(idx(col,row) < pixel_count_ && ZERR_IDX_OUT_OF_RANGE);
            return *(reinterpret_cast<pixel_t*>(mapped_ptr_) + idx(col,row));
        }

        pixel_t& operator()(size_t col, size_t row) {
            assert(is_mapped() && "Pixel Buffer must be mapped!");
            assert(idx(col,row) < pixel_count_ && ZERR_IDX_OUT_OF_RANGE);
            return *(reinterpret_cast<pixel_t*>(mapped_ptr_) + idx(col,row));
        }

    private:
        size_t pixel_count_, width_, height_, depth_;
        mutable bool write_bound_;
    };
}}

#endif //ZAP_PIXEL_BUFFER_HPP

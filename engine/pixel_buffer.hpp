/* Created by Darren Otgaar on 2016/04/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_PIXEL_BUFFER_HPP
#define ZAP_PIXEL_BUFFER_HPP

#include "buffer.hpp"
#include "pixmap.hpp"
#include "pixel_format.hpp"

namespace zap { namespace engine {
    template <typename PIX_T>
    class pixel_buffer : public buffer {
    public:
        using pixel_t = PIX_T;
        using pixmap_t = pixmap<pixel_t>;
        constexpr static auto read_type = buffer_type::BT_PIXEL_PACK;
        constexpr static auto write_type = buffer_type::BT_PIXEL_UNPACK;

        explicit pixel_buffer(buffer_usage use=buffer_usage::BU_STREAM_COPY) : buffer(use) { }
        pixel_buffer(const pixel_buffer&) = delete;
        pixel_buffer(pixel_buffer&& rhs) noexcept : buffer(std::move(rhs)), pixel_count_(rhs.pixel_count_) { }
        ~pixel_buffer() override = default;

        pixel_buffer& operator=(const pixel_buffer&) = delete;
        pixel_buffer& operator=(pixel_buffer&& rhs) noexcept {
            if(this != &rhs) {
                buffer::operator=(std::move(rhs));
                pixel_count_ = rhs.pixel_count_;
            }
            return *this;
        }

        size_t idx(size_t col, size_t row) const { return row*width_ + col; }
        size_t idx(size_t col, size_t row, size_t dep) const { return width_*(row + dep*height_) + col; }

        size_t width() const { return width_; }
        size_t height() const { return height_; }
        size_t depth() const { return depth_; }
        size_t pixel_count() const { return pixel_count_; }
        size_t pixel_size() const { return sizeof(pixel_t); }

        void bind(buffer_type type=write_type) const { buffer::bind(type); }
        void release(buffer_type type=write_type) const { buffer::release(type); }

        bool initialise(size_t width, size_t height, buffer_type type, const pixel_t* data=nullptr) {
            if(buffer::initialise(type, usage_, width*height*pixel_t::bytesize, reinterpret_cast<const char*>(data))) {
                pixel_count_ = width*height; width_ = width; height_ = height; depth_ = 1;
                return true;
            }
            return false;
        }

        bool initialise(size_t width, size_t height, const pixel_t* data=nullptr) {
            if(buffer::initialise(write_type, usage_, width*height*pixel_t::bytesize, reinterpret_cast<const char*>(data))) {
                pixel_count_ = width*height; width_ = width; height_ = height; depth_ = 1;
                return true;
            }
            return false;
        }

        bool resize(size_t width, size_t height) {
            if(buffer::initialise(write_type, usage_, width*height*pixel_t::bytesize)) {
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
                bind();
                buffer::copy(write_type, trg_offset, length, reinterpret_cast<const char*>(buffer.data())+src_offset);
                release();
                return pixel_count;
            }
            return 0;
        }

        size_t copy(const size_t src_off, const size_t trg_off, size_t length) {
            assert(is_mapped() && "Pixel Buffer must be mapped!");
            std::copy(data()+src_off, data()+src_off+length, data()+trg_off);
            return length;
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

        const pixel_t* data() const { return (reinterpret_cast<pixel_t*>(mapped_ptr_)); }
        pixel_t* data() { return (reinterpret_cast<pixel_t*>(mapped_ptr_)); }

    private:
        size_t pixel_count_ = 0;
        size_t width_ = 0;
        size_t height_ = 0;
        size_t depth_ = 0;
        mutable bool write_bound_ = false;
    };
}}

#endif //ZAP_PIXEL_BUFFER_HPP

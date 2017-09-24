/* Created by Darren Otgaar on 2016/04/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_VERTEX_BUFFER_HPP
#define ZAP_VERTEX_BUFFER_HPP

#include "buffer.hpp"
#include <core/pod.hpp>
#include "vertex_format.hpp"

namespace zap { namespace engine {

namespace gl {
    void vertex_attrib_ptr(uint32_t index, int32_t size, data_type type, bool normalized, uint32_t stride, const void* ptr);
    void vertex_attrib_iptr(uint32_t index, int32_t size, data_type type, uint32_t stride, const void* ptr);
    void vertex_attrib_lptr(uint32_t index, int32_t size, data_type type, uint32_t stride, const void* ptr);
}

template <typename VTX_T>
class vertex_buffer : public buffer {
    static_assert(is_specialisation_of<vertex, VTX_T>::value, "VTX_T must be a specialisation of vertex<>");

public:
    using vertex_t = VTX_T;
    using iterator = vertex_iterator<VTX_T>;
    using pod_t = typename vertex_t::pod_t;
    constexpr static auto buf_type = buffer_type::BT_ARRAY;

    explicit vertex_buffer(buffer_usage use=buffer_usage::BU_STATIC_DRAW) : buffer(use) { }
    vertex_buffer(const vertex_buffer&) = delete;
    vertex_buffer(vertex_buffer&& rhs) noexcept : buffer(std::move(rhs)), vertex_count_(rhs.vertex_count_) { }
    virtual ~vertex_buffer() = default;

    vertex_buffer& operator=(const vertex_buffer&) = delete;
    vertex_buffer& operator=(vertex_buffer&& rhs) noexcept {
        if(this != &rhs) {
            buffer::operator=(std::move(rhs));
            vertex_count_ = rhs.vertex_count_;
        }
        return *this;
    }

    void bind() const { buffer::bind(buf_type); }
    void release() const { buffer::release(buf_type); }

    bool initialise(size_t vertex_count, const char* data=nullptr) {
        if(buffer::initialise(buf_type, usage(), vertex_count*vertex_t::bytesize(), data)) {
            vertex_count_ = vertex_count;
            return configure_attributes();
        }
        return false;
    }

    bool initialise(const std::vector<char>& data) {
        if(buffer::initialise(buf_type, usage(), data)) {
            vertex_count_ = data.size() / vertex_t::bytesize();
            return configure_attributes();
        }
        return false;
    }

    bool initialise(const std::vector<vertex_t>& data) {
        auto result = buffer::initialise(buf_type, usage(), vertex_t::bytesize()*data.size(),
                                         reinterpret_cast<const char*>(data.data()));
        vertex_count_ = data.size();
        return result && configure_attributes();
    }

    bool initialise(size_t vertex_count, const vertex_t* data) { return initialise(vertex_count, reinterpret_cast<const char*>(data)); }

    bool orphan() { return buffer::orphan(buf_type, usage()); }

    // All sizes are in vertices, i.e src_off = 0 is the first vertex, src_off = 1 is the second and so on.
    bool copy(const vertex_buffer& src, size_t src_off, size_t trg_off, size_t vertex_count);
    bool copy(const std::vector<vertex_t>& data, size_t offset, size_t vertex_count) {
        return buffer::copy(buf_type, offset, vertex_t::bytesize()*data.size(), reinterpret_cast<const char*>(data.data()));
    }
    bool copy(const char* data, size_t offset, size_t count) {
        return buffer::copy(buf_type, offset, count, data);
    }

    char* map(buffer_access access) { return buffer::map(buf_type, access); }
    char* map(range_access::code access, size_t offset, size_t length) {
        assert(offset < vertex_count_ && offset+length <= vertex_count_ && ZERR_IDX_OUT_OF_RANGE);
        return buffer::map(buf_type, access, offset*vertex_t::bytesize(), length*vertex_t::bytesize());
    }
    void flush(size_t offset, size_t length) { return buffer::flush(buf_type, offset*vertex_t::bytesize(), length*vertex_t::bytesize()); }
    bool unmap() { return buffer::unmap(buf_type); }

    const vertex_t& operator[](size_t idx) const {
        assert(is_mapped() && "Vertex Buffer must be mapped!");
        assert(idx < vertex_count_ && ZERR_IDX_OUT_OF_RANGE);
        return *(reinterpret_cast<vertex_t*>(mapped_ptr_) + idx);
    }

    vertex_t& operator[](size_t idx) {
        assert(is_mapped() && "Vertex Buffer must be mapped!");
        assert(idx < vertex_count_ && ZERR_IDX_OUT_OF_RANGE);
        return *(reinterpret_cast<vertex_t*>(mapped_ptr_) + idx);
    }

    const vertex_t& at(size_t idx) const {
        return operator[](idx);
    }

    vertex_t& at(size_t idx) {
        return operator[](idx);
    }

    iterator begin() {
        assert(is_mapped() && "Vertex Buffer must be mapped!");
        return iterator(reinterpret_cast<vertex_t*>(mapped_ptr_));
    }
    iterator end() {
        assert(is_mapped() && "Vertex Buffer must be mapped!");
        return iterator(reinterpret_cast<vertex_t*>(mapped_ptr_ + (size_ / vertex_t::bytesize()) * vertex_t::bytesize()));
    }
    const iterator begin() const {
        assert(is_mapped() && "Vertex Buffer must be mapped!");
        return iterator(reinterpret_cast<vertex_t*>(mapped_ptr_));
    }
    const iterator end() const {
        return iterator(reinterpret_cast<vertex_t*>(mapped_ptr_ + (size_ / vertex_t::bytesize()) * vertex_t::bytesize()));
    }

    vertex_t* data() {
        assert(is_mapped() && "Vertex Buffer must be mapped!");
        return reinterpret_cast<vertex_t*>(mapped_ptr_);
    }
    const vertex_t* data() const {
        assert(is_mapped() && "Vertex Buffer must be mapped!");
        return reinterpret_cast<vertex_t*>(mapped_ptr_);
    }

    size_t vertex_count() const { return vertex_count_; }
    size_t capacity() const { return size() / vertex_t::bytesize(); }

    bool configure_attributes();

private:
    size_t vertex_count_ = 0;
};

template <typename VTX_T>
bool vertex_buffer<VTX_T>::configure_attributes() {
    static_assert(vertex_t::size > 0, "An empty vertex<> type cannot be configured");

    for(size_t i = 0; i != vertex_t::size; ++i) {
        LOG("Vertex Attribute Binding", vertex_t::types::data[i], vertex_t::counts::data[i], vertex_t::datatypes::data[i],
            vertex_t::offsets::data[i], vertex_t::is_int::data[i]);
        if(vertex_t::is_int::data[i])
            gl::vertex_attrib_iptr(uint32_t(vertex_t::types::data[i]), int32_t(vertex_t::counts::data[i]),
                                   (data_type)vertex_t::datatypes::data[i], uint32_t(vertex_t::bytesize()),
                                   (void*)vertex_t::offsets::data[i]);
        else
            gl::vertex_attrib_ptr(uint32_t(vertex_t::types::data[i]), int32_t(vertex_t::counts::data[i]),
                                  (data_type)vertex_t::datatypes::data[i], false, uint32_t(vertex_t::bytesize()),
                                  (void*)vertex_t::offsets::data[i]);
    }
    if(gl_error_check()) return false;

    return true;
}

template <typename VTX_T>
bool vertex_buffer<VTX_T>::copy(const vertex_buffer& src, size_t src_off, size_t trg_off, size_t vertex_count) {
    const size_t length = vertex_count * VTX_T::bytesize();
    const size_t source_offset = src_off * VTX_T::bytesize();
    const size_t target_offset = trg_off * VTX_T::bytesize();
    if(source_offset + length <= src.size_ && target_offset + length <= size_) {
        src.buffer::bind(buffer_type::BT_COPY_READ);
        buffer::bind(buffer_type::BT_COPY_WRITE);
        copy_buffer(buffer_type::BT_COPY_READ, buffer_type::BT_COPY_WRITE, source_offset, target_offset, length);
        buffer::release(buffer_type::BT_COPY_WRITE);
        src.buffer::release(buffer_type::BT_COPY_READ);
        return true;
    }
    return false;
}

template <typename Parm> struct is_vertex_buffer : std::false_type { };
template <typename VTX_T> struct is_vertex_buffer<vertex_buffer<VTX_T>> : std::true_type { };

}}

#endif //ZAP_VERTEX_BUFFER_HPP

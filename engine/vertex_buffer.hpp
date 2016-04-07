/* Created by Darren Otgaar on 2016/04/06. http://www.github.com/otgaard/zap */
#ifndef ZAP_VERTEX_BUFFER_HPP
#define ZAP_VERTEX_BUFFER_HPP

#include "buffer.hpp"
#include "buffer_format.hpp"

namespace zap { namespace engine {

namespace gl {
    void vertex_attrib_ptr(uint32_t index, int32_t size, data_type type, bool normalized, uint32_t stride, const void* ptr);
    void vertex_attrib_iptr(uint32_t index, int32_t size, data_type type, uint32_t stride, const void* ptr);
    void vertex_attrib_lptr(uint32_t index, int32_t size, data_type type, uint32_t stride, const void* ptr);
}

template <typename VTX_T, buffer_usage USAGE>
class vertex_buffer : public buffer {
public:
    static_assert(is_specialisation_of<vertex, VTX_T>::value, "VTX_T must be a specialisation of vertex<>");

    using vertex_t = VTX_T;
    using pod_t = typename vertex_t::pod_t;
    constexpr static auto buf_type = buffer_type::BT_ARRAY;
    constexpr static auto usage = USAGE;

    inline vertex_buffer() : vertex_count_(0) { }

    inline void bind() { buffer::bind(buf_type); }
    inline void release() { buffer::release(buf_type); }

    inline bool initialise(size_t size, const char* data=nullptr) { buffer::initialise(buf_type, usage, size, data); }
    inline bool initialise(const std::vector<char>& data) { buffer::initialise(buf_type, usage, data); }
    inline bool initialise(const std::vector<vertex_t>& data) {
        auto result = buffer::initialise(buf_type, usage, vertex_t::size()*data.size(),
                                         reinterpret_cast<const char*>(data.data()));
        vertex_count_ = data.size();
        return result && configure_attributes();
    }

    inline char* map(buffer_access::bitfield access) { return buffer::map(buf_type, access); }
    inline char* map(buffer_access::bitfield access, size_t offset, size_t length) {
        return buffer::map(buf_type, access, offset, length);
    }
    inline bool unmap() { return buffer::unmap(buf_type); }

    inline size_t vertex_count() const { return vertex_count_; }

protected:
    bool configure_attributes();

    size_t vertex_count_;
};

template <typename VTX_T, buffer_usage USAGE>
bool vertex_buffer<VTX_T,USAGE>::configure_attributes() {
    static_assert(vertex_t::attrib_count() > 0, "An empty vertex<> type cannot be configured");

    LOG_WARN("*** hardcoded values here ***");

    LOG(type_query<0,pod_t>::type::type::size(), type_query<1,pod_t>::type::type::size());
    LOG(vertex_t::template attrib_offset<0>(), vertex_t::template attrib_offset<1>());

    gl::vertex_attrib_ptr(0, type_query<0,pod_t>::type::type::size(), data_type::DT_FLOAT, false, vertex_t::size(),
                          (void*)vertex_t::template attrib_offset<0>());
    gl::vertex_attrib_ptr(1, type_query<1,pod_t>::type::type::size(), data_type::DT_FLOAT, false, vertex_t::size(),
                          (void*)vertex_t::template attrib_offset<1>());
    return true;
}

}}

#endif //ZAP_VERTEX_BUFFER_HPP

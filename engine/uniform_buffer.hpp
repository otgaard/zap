//
// Created by Darren Otgaar on 2016/05/15.
//

#ifndef ZAP_UNIFORM_BUFFER_HPP
#define ZAP_UNIFORM_BUFFER_HPP

#include "buffer.hpp"
#include "uniform_block.hpp"

namespace zap { namespace engine {

namespace gl {
    void bind_buffer_base(buffer_type type, int idx, uint32_t bo);
}

class ubuffer_base : public buffer {
public:
    constexpr static auto buf_type = buffer_type::BT_UNIFORM;

    explicit ubuffer_base(buffer_usage use=buffer_usage::BU_STREAM_COPY) : buffer(use) { }
    ~ubuffer_base() override = default;

    void bind() const { buffer::bind(buf_type); }
    void release() const { buffer::release(buf_type); }

    void bind_point(int location) const { gl::bind_buffer_base(buf_type, location, id_); }
};

template <typename UBlock>
class uniform_buffer : public ubuffer_base {
    static_assert(is_specialisation_of<uniform_block, UBlock>::value, "UBlock must be a specialisation of uniform_block<>");

public:
    using block_t = UBlock;
    using pod_t = typename block_t::pod_t;

    explicit uniform_buffer(buffer_usage use=buffer_usage::BU_STREAM_COPY) : ubuffer_base(use) { }
    ~uniform_buffer() override = default;

    bool initialise(const char* data=nullptr) {
        return buffer::initialise(buf_type, usage(), block_t::bytesize(), data);
    }

    bool initialise(const block_t& data) {
        return buffer::initialise(buf_type, usage(), block_t::bytesize(), reinterpret_cast<const char*>(&data));
    }

    char* map(buffer_access access) { return buffer::map(buf_type, access); }
    char* map(buffer_access access, size_t offset, size_t length) {
        return buffer::map(buf_type, access, offset, length);
    }
    bool unmap() { return buffer::unmap(buf_type); }

    const block_t* get() const {
        assert(is_mapped() && "Uniform Buffer must be mapped!");
        return reinterpret_cast<block_t*>(mapped_ptr_);
    }

    block_t* get() {
        assert(is_mapped() && "Uniform Buffer must be mapped!");
        return reinterpret_cast<block_t*>(mapped_ptr_);
    }
};

}}

#endif //ZAP_UNIFORM_BUFFER_HPP

/* Created by Darren Otgaar on 2016/03/27. http://www.github.com/otgaard/zap */
#ifndef ZAP_BUFFER_HPP
#define ZAP_BUFFER_HPP

#include <array>
#include <vector>
#include "engine.hpp"

namespace zap { namespace engine {
    static const char* const ZERR_UNALLOCATED_BUFFER = "buffer is not allocated";

    class buffer {
    public:
        buffer() : id_(INVALID_RESOURCE), mapped_ptr_(nullptr), size_(0) { }
        ~buffer();

        bool allocate();
        bool deallocate();

        void bind(buffer_type type) const;
        void release(buffer_type type) const;
        bool is_bound() const;

        bool is_mapped() const { return mapped_ptr_ != nullptr; }
        bool is_allocated() const { return id_ != INVALID_RESOURCE; }

        bool initialise(buffer_type type, buffer_usage usage, size_t size, const char* data=nullptr);
        bool initialise(buffer_type type, buffer_usage usage, const std::vector<char>& data) {
            return initialise(type, usage, data.size(), data.data());
        }

        bool orphan(buffer_type type, buffer_usage usage);

        bool copy(buffer_type type, size_t offset, size_t size, const char* data); // glBufferSubData
        bool copy(buffer_type type, size_t offset, const std::vector<char>& data) {
            return copy(type, offset, data.size(), data.data());
        }

        const char* map(buffer_type type) const;    // GL_READ_ONLY
        char* map(buffer_type type, buffer_access access); // glMapBuffer
        char* map(buffer_type type, buffer_access access, size_t offset, size_t length); // glMapBufferRange
        bool unmap(buffer_type type) const; // glUnmapBuffer

        bool copy_buffer(buffer_type src_type, buffer_type trg_type, size_t src_offset, size_t trg_offset, size_t length);

    protected:
        mutable resource_t id_;
        mutable char* mapped_ptr_;
        size_t size_;
    };
}}

#endif //ZAP_BUFFER_HPP

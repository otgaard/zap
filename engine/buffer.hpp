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
        inline buffer() : id_(INVALID_RESOURCE), size_(0), mapped_ptr_(nullptr) { }
        ~buffer();

        bool allocate();
        bool deallocate();

        void bind(buffer_type type);
        void release(buffer_type type);
        bool is_bound() const;

        inline bool is_mapped() const { return mapped_ptr_ != nullptr; }
        inline bool is_allocated() const { return id_ != INVALID_RESOURCE; }

        bool initialise(buffer_type type, buffer_usage usage, size_t size, const char* data=nullptr);
        inline bool initialise(buffer_type type, buffer_usage usage, const std::vector<char>& data) {
            return initialise(type, usage, data.size(), data.data());
        }

        bool copy(buffer_type type, size_t offset, size_t size, const char* data); // glBufferSubData
        inline bool copy(buffer_type type, size_t offset, const std::vector<char>& data) {
            return copy(type, offset, data.size(), data.data());
        }

        char* map(buffer_type type, buffer_access::bitfield access); // glMapBuffer
        char* map(buffer_type type, buffer_access::bitfield access, size_t offset, size_t length); // glMapBufferRange
        bool unmap(buffer_type type); // glUnmapBuffer

    protected:
        resource_t id_;
        size_t size_;
        char* mapped_ptr_;
    };
}}

#endif //ZAP_BUFFER_HPP

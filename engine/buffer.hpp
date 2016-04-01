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
        inline ~buffer() {
            if(is_mapped()) unmap();
            if(is_allocated()) deallocate();
        }

        inline bool allocate() {
            gl::glGenBuffers(1, &id_);
            gl_error_check();
            return is_allocated();
        }

        inline bool deallocate() {
            assert(!is_mapped() && is_allocated() && "buffer is still mapped or is unallocated");
            if(is_mapped()) return false;

            gl::glDeleteBuffers(1, &id_);
            id_ = INVALID_RESOURCE;
            size_ = 0;
            return true;
        }

        inline void bind(buffer_type type) {
            assert(is_allocated() && ZERR_UNALLOCATED_BUFFER);
            gl::glBindBuffer(gl::gl_type(type), id_);
            gl_error_check();
        }

        inline void release(buffer_type type) {
            gl::glBindBuffer(gl::gl_type(type), 0);
        }

        inline bool is_mapped() const { return mapped_ptr_ != nullptr; }
        inline bool is_allocated() const { return id_ != INVALID_RESOURCE; }

        inline bool initialise(buffer_type type, buffer_usage usage, size_t size, char* data=nullptr) {
            assert(is_allocated() && ZERR_UNALLOCATED_BUFFER);
            gl::glBufferData(gl::gl_type(type), size, data, gl::gl_type(usage));
            return gl_error_check();
        }

        bool initialise(buffer_type type, buffer_usage usage, const std::vector<char>& data); // glBufferData
        bool copy(buffer_type type, size_t offset, size_t size, char* data); // glBufferSubData
        bool copy(buffer_type type, size_t offset, const std::vector<char>& data); // glBufferSubData
        char* map(buffer_type type, buffer_access access); // glMapBuffer
        char* map(buffer_type type, buffer_access access, size_t offset, size_t length); // glMapBufferRange
        bool unmap(); // glUnmapBuffer

    protected:
        resource_t id_;
        size_t size_;
        char* mapped_ptr_;
        std::vector<char> local_;
    };


}}

#endif //ZAP_BUFFER_HPP

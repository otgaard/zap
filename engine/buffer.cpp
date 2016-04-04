/* Created by Darren Otgaar on 2016/03/27. http://www.github.com/otgaard/zap */
#include "buffer.hpp"

using namespace zap::engine;
using namespace zap::engine::gl;

buffer::~buffer() {
    assert(!is_mapped());
    if(is_allocated()) deallocate();
}

bool buffer::initialise(buffer_type type, buffer_usage usage, size_t size, const char* data) {
    assert(is_allocated() && ZERR_UNALLOCATED_BUFFER);
    glBufferData(gl_type(type), size, data, gl_type(usage));
    if(gl_error_check()) return false;
    size_ = size;
    return true;
}

bool buffer::copy(buffer_type type, size_t offset, size_t size, const char* data) {
    assert(is_allocated() && ZERR_UNALLOCATED_BUFFER);
    glBufferSubData(gl_type(type), offset, size, data);
    return !gl_error_check();
}

char* buffer::map(buffer_type type, buffer_access access) {
    assert(is_allocated() && ZERR_UNALLOCATED_BUFFER);
    mapped_ptr_ = reinterpret_cast<char*>(glMapBuffer(gl_type(type), gl_type(access)));
    if(mapped_ptr_ == nullptr || gl_error_check()) {
        unmap(type);
        return nullptr;
    }
    return mapped_ptr_;
}

char* buffer::map(buffer_type type, buffer_access access, size_t offset, size_t length) {
    assert(is_allocated() && ZERR_UNALLOCATED_BUFFER);
    mapped_ptr_ = reinterpret_cast<char*>(glMapBufferRange(gl_type(type), offset, length, gl_type(access)));
    if(mapped_ptr_ == nullptr || gl_error_check()) {
        unmap(type);
        return nullptr;
    }
    return mapped_ptr_;
}

bool buffer::unmap(buffer_type type) {
    assert(is_allocated() && is_mapped() && "Buffer unallocated or unmapped");
    glUnmapBuffer(gl_type(type));
    return !gl_error_check();
}

/* Created by Darren Otgaar on 2016/03/27. http://www.github.com/otgaard/zap */
#include "buffer.hpp"
#include "gl_api.hpp"

using namespace zap::engine;
using namespace zap::engine::gl;

buffer::~buffer() {
    assert(!is_mapped());
    if(is_allocated()) deallocate();
}

bool buffer::allocate() {
    gl::glGenBuffers(1, &id_);
    gl_error_check();
    return is_allocated();
}

bool buffer::deallocate() {
    assert(!is_mapped() && is_allocated() && "buffer is still mapped or is unallocated");
    if(is_mapped()) return false;

    gl::glDeleteBuffers(1, &id_);
    id_ = INVALID_RESOURCE;
    size_ = 0;
    return true;
}

void buffer::bind(buffer_type type) {
    assert(is_allocated() && ZERR_UNALLOCATED_BUFFER);
    gl::glBindBuffer(gl::gl_type(type), id_);
    gl_error_check();
}

void buffer::release(buffer_type type) {
    gl::glBindBuffer(gl::gl_type(type), 0);
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

char* buffer::map(buffer_type type, buffer_access::bitfield access) {
    assert(is_allocated() && ZERR_UNALLOCATED_BUFFER);
    mapped_ptr_ = reinterpret_cast<char*>(glMapBuffer(gl_type(type), gl_type(access)));
    if(mapped_ptr_ == nullptr || gl_error_check()) {
        unmap(type);
        return nullptr;
    }
    return mapped_ptr_;
}

char* buffer::map(buffer_type type, buffer_access::bitfield access, size_t offset, size_t length) {
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

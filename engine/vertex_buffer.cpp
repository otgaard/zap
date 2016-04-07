//
// Created by Darren Otgaar on 2016/04/07.
//

#include "vertex_buffer.hpp"
#include "gl_api.hpp"

using namespace zap::engine;

void gl::vertex_attrib_ptr(uint32_t index, int32_t size, data_type type, bool normalized, uint32_t stride, const void* ptr) {
    LOG_WARN("Incomplete Implementation", __FILE__, __LINE__);
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, (GLboolean)normalized, stride, ptr);
    gl_error_check();
}

void gl::vertex_attrib_iptr(uint32_t index, int32_t size, data_type type, uint32_t stride, const void* ptr) {
    LOG_WARN("Incomplete Implementation", __FILE__, __LINE__);
    glEnableVertexAttribArray(index);
    glVertexAttribIPointer(index, size, GL_FLOAT, stride, ptr);
    gl_error_check();
}

void gl::vertex_attrib_lptr(uint32_t index, int32_t size, data_type type, uint32_t stride, const void* ptr) {
    LOG_WARN("Incomplete Implementation", __FILE__, __LINE__);
    glEnableVertexAttribArray(index);
    glVertexAttribLPointer(index, size, GL_FLOAT, stride, ptr);
    gl_error_check();
}

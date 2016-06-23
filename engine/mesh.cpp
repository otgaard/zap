/* Created by Darren Otgaar on 2016/04/16. http://www.github.com/otgaard/zap */
#include "mesh.hpp"
#include "gl_api.hpp"

namespace zap { namespace engine {

using namespace gl;

bool mesh_base::allocate() {
    glGenVertexArrays(1, &vao_);
    LOG("Mesh Allocated: ", vao_);
    return is_allocated() && !gl_error_check();
}

void mesh_base::deallocate() {
    glDeleteVertexArrays(1, &vao_);
    LOG("Mesh Deallocated:", vao_);
    vao_ = INVALID_RESOURCE;
}

void mesh_base::bind() const {
    glBindVertexArray(vao_);
    gl_error_check();
}

void mesh_base::release() const {
    glBindVertexArray(0);
    gl_error_check();
}

void mesh_base::draw_arrays_impl(primitive_type type, size_t first, size_t count) {
    glDrawArrays(gl_type(type), first, count);
}

void mesh_base::draw_elements_impl(primitive_type type, data_type index_type, size_t count) {
    glDrawElements(gl_type(type), count, gl_type(index_type), nullptr);
}

}}

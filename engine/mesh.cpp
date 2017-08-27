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

void mesh_base::draw_arrays_impl(primitive_type type, uint32_t first, uint32_t count) const {
    glDrawArrays(gl_type(type), first, count);
}

void mesh_base::draw_arrays_inst_impl(primitive_type type, uint32_t first, uint32_t count, uint32_t instances) const {
    glDrawArraysInstanced(gl_type(type), first, count, instances);
}

void mesh_base::draw_elements_impl(primitive_type type, data_type index_type, uint32_t first, uint32_t count) const {
    glDrawElements(gl_type(type), count, gl_type(index_type), reinterpret_cast<void*>(first*dt_bytesize(index_type)));
}

void mesh_base::draw_elements_inst_impl(primitive_type type, data_type index_type, uint32_t first, uint32_t count, uint32_t instances) const {
    glDrawElementsInstanced(gl_type(type), count, gl_type(index_type), reinterpret_cast<void*>(first*dt_bytesize(index_type)), instances);
}

void mesh_base::set_attrib_divisor(uint32_t va_idx, uint32_t divisor) {
    glVertexAttribDivisor(va_idx, divisor);
}

void mesh_base::draw_arrays_inst_baseinst_impl(primitive_type type, uint32_t first, uint32_t count, uint32_t instances,
                                               uint32_t offset) const {
    glDrawArraysInstancedBaseInstance(gl_type(type), first, count, instances, offset);
}

void
mesh_base::draw_elements_inst_baseinst_impl(primitive_type type, data_type index_type, uint32_t first, uint32_t count,
                                            uint32_t instances, uint32_t offset) const {
    glDrawElementsInstancedBaseInstance(gl_type(type), first, gl_type(index_type),
                                        reinterpret_cast<void*>(first*dt_bytesize(index_type)), instances, offset);
}

}}

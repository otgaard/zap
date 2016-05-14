/* Created by Darren Otgaar on 2016/04/16. http://www.github.com/otgaard/zap */
#include "mesh.hpp"
#include "gl_api.hpp"

namespace zap { namespace engine {

using namespace gl;

void mesh_base::allocate() {
    glGenVertexArrays(1, &vao_);
    gl_error_check();
}

void mesh_base::bind() {
    glBindVertexArray(vao_);
    gl_error_check();
}

void mesh_base::release() {
    glBindVertexArray(0);
    gl_error_check();
}

void mesh_base::draw(primitive_type type, size_t first, size_t count) {
    glDrawArrays(gl_type(type), first, count);
}

}}
//
// Created by Darren Otgaar on 2016/03/22.
//

#include "shader.hpp"
#include "gl_api.hpp"
#include <maths/maths.hpp>

using namespace zap::engine;

shader::~shader() {
    if(is_compiled()) gl::glDeleteShader(id_);
}

bool shader::compile(bool clear) {
    using namespace gl;

    id_ = glCreateShader(gl_type(type_));
    if(!is_allocated()) {
        LOG_ERR("Failed to create shader of type", gl_type(type_));
        return compiled_;
    }

    const char* source[1] = {src_.c_str()};
    glShaderSource(id_, 1, source, nullptr);
    glCompileShader(id_);

    GLint success;
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    if(success == GL_FALSE) {
        GLint length;
        glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &length);

        std::string msg(size_t(maths::max(length, 0)), ' ');
        glGetShaderInfoLog(id_, length, &length, &msg[0]);

        LOG_ERR("Compilation error in", gl_typename(type_), ":\n", msg);
        glDeleteShader(id_);
        id_ = INVALID_RESOURCE;
        return compiled_;
    }

    if(clear) src_.clear();
    compiled_ = true;
    return compiled_;
}

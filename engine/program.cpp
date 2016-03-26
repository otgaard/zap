//
// Created by Darren Otgaar on 2016/03/25.
//

#include <maths/maths.hpp>
#include "program.hpp"
#include "gl_api.hpp"

using namespace zap::engine;

program::~program() {
    if(is_linked()) gl::glDeleteProgram(id_);
}

std::int32_t program::uniform_location(const std::string& name) {
    return gl::glGetUniformLocation(id_, name.c_str());
}

void program::bind() {
    gl::glUseProgram(id_);
}

void program::release() {
    gl::glUseProgram(0);
}

bool program::link(bool clear) {
    using namespace gl;

    id_ = glCreateProgram();
    if(!is_allocated()) {
        LOG_ERR("Failed to allocate program.");
        return linked_;
    }

    for(const auto& shdr : shaders_) {
        if(!shdr->is_compiled()) shdr->compile(clear);
        if(shdr->is_compiled()) glAttachShader(id_, shdr->resource());
        else LOG_ERR("Shader failed to compile during program construction");
    }

    // Specify Attribute Location Parameters Here
    glBindAttribLocation(id_, 0, "position");

    glLinkProgram(id_);
    GLint success;
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if(success == GL_FALSE) {
        GLint length;
        glGetProgramiv(id_, GL_INFO_LOG_LENGTH, &length);

        std::string msg(size_t(maths::max(length, 0)), ' ');
        glGetProgramInfoLog(id_, length, &length, &msg[0]);

        LOG_ERR("Linker error at:", msg);
        glDeleteProgram(id_);
        id_ = INVALID_RESOURCE;
        return linked_;
    }

    if(clear) shaders_.clear();
    linked_ = true;
    return linked_;
}

//
// Created by Darren Otgaar on 2016/03/25.
//

#include "program.hpp"
#include "gl_api.hpp"

using namespace zap::engine;

program::~program() {
    if(is_linked()) gl::glDeleteProgram(id_);
}

std::int32_t program::uniform_location(const char* name) {
    return gl::glGetUniformLocation(id_, name);
}

std::int32_t program::uniform_block_index(const char* name) {
    return gl::glGetUniformBlockIndex(id_, name);
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

    for(size_t i = 0; i != size_t(attribute_type::AT_SIZE); ++i) {
        glBindAttribLocation(id_, i, attribute_name[i]);
    }

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

void program::bind_texture_unit(int location, int unit) {
    gl::glUniform1i(location, unit);
}

template <> void zap::engine::program::bind_uniform<int>(int location, const int& value) {
    gl::glUniform1i(location, value);
}

template <> void zap::engine::program::bind_uniform<float>(int location, const float& value) {
    gl::glUniform1f(location, value);
}

template <> void zap::engine::program::bind_uniform<zap::maths::vec3f>(int location, const zap::maths::vec3f& type) {
    gl::glUniform3fv(location, 1, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::mat3f>(int location, const zap::maths::mat3f& type) {
    gl::glUniformMatrix3fv(location, 1, GL_FALSE, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::mat4f>(int location, const zap::maths::mat4f& type) {
    gl::glUniformMatrix4fv(location, 1, GL_FALSE, type.data());
}

template <> void zap::engine::program::bind_uniform<float>(const char* name, const float& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid mat4f uniform specified");
    if(loc != -1) gl::glUniform1f(loc, type);
}

template <> void zap::engine::program::bind_uniform<zap::maths::vec3f>(const char* name, const zap::maths::vec3f& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid mat4f uniform specified");
    if(loc != -1) gl::glUniform3fv(loc, 1, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::mat3f>(const char* name, const zap::maths::mat3f& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid mat3f uniform specified");
    if(loc != -1) gl::glUniformMatrix3fv(loc, 1, GL_FALSE, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::mat4f>(const char* name, const zap::maths::mat4f& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid mat4f uniform specified");
    if(loc != -1) gl::glUniformMatrix4fv(loc, 1, GL_FALSE, type.data());
}

template <> void zap::engine::program::bind_uniform<int>(const char* name, const int& value) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid mat4f uniform specified");
    if(loc != -1) gl::glUniform1i(loc, value);
}

//
// Created by Darren Otgaar on 2016/03/25.
//

#include "program.hpp"
#include "gl_api.hpp"

using namespace zap::engine;

program::~program() {
    if(is_linked()) gl::glDeleteProgram(id_);
    LOG("Program Deallocated:", id_);
}

std::int32_t program::uniform_location(const char* name) {
    return gl::glGetUniformLocation(id_, name);
}

std::int32_t program::uniform_block_index(const char* name) {
    return gl::glGetUniformBlockIndex(id_, name);
}


void program::bind() const {
    gl::glUseProgram(id_);
}

void program::release() const {
    gl::glUseProgram(0);
}

bool program::link(bool clear, bool bind_generic) {
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

    for(uint32_t i = 0; i != uint32_t(attribute_type::AT_SIZE); ++i) {
        if(bind_generic) glBindAttribLocation(id_, i, generic_attribute_name[i]);       // loc0, loc1, etc
        else             glBindAttribLocation(id_, i, attribute_name[i]);               // position, normal, etc
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

bool program::link(const char* const vshdr, const char* const fshdr, bool clear, bool bind_generic) {
    add_shader(shader_type::ST_VERTEX, vshdr);
    add_shader(shader_type::ST_FRAGMENT, fshdr);
    return link(clear, bind_generic);
}
bool program::link(const char* const vshdr, const char* const gshdr, const char* const fshdr, bool clear, bool bind_generic) {
    add_shader(shader_type::ST_VERTEX, vshdr);
    add_shader(shader_type::ST_GEOMETRY, gshdr);
    add_shader(shader_type::ST_FRAGMENT, fshdr);
    return link(clear, bind_generic);
}

void program::bind_texture_unit(int location, int unit) {
    gl::glUniform1i(location, unit);
}

void program::bind_texture_unit(const char* name, int unit) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid sampler uniform specified");
    if(loc != -1) gl::glUniform1i(loc, unit);
}

std::vector<parameter> program::get_parameters() const {
    const uint32_t bufsize = 128;
    char buffer[bufsize] = {0};

    std::vector<parameter> parms{};

    // Query the uniforms

    int32_t uniform_count;
    gl::glGetProgramiv(id_, GL_ACTIVE_UNIFORMS, &uniform_count);
    for(int i = 0; i != uniform_count; ++i) {
        parameter p{-1};
        int len;
        gl::GLenum gltype;

        gl::glGetActiveUniform(id_, i, bufsize, &len, &p.count, &gltype, buffer);
        if(gl::internal_type(gltype, gl::gl_parameter_type, gl::gl_parameter_type+int(parameter_type::PT_SIZE), p.type)) {
            p.name = std::string{buffer, size_t(len)};
            p.location = gl::glGetUniformLocation(id_, p.name.c_str());
            if(p.location != -1) {
                LOG("Parameter:", p.location, LOG_BLUE, p.name, LOG_YELLOW, gl::gl_typename(p.type), LOG_GREEN, "count:", p.count);
                parms.emplace_back(p);
            }
        } else {
            LOG_ERR("Unsupported uniform type:", i, buffer);
        }
    }

    gl_error_check();
    return parms;
}

std::vector<block> program::get_uniform_blocks() const {
    const uint32_t bufsize = 128;
    char buffer[bufsize] = {0};

    std::vector<block> uni_blocks{};

    // Query the uniform buffers
    int32_t block_count;
    gl::glGetProgramiv(id_, GL_ACTIVE_UNIFORM_BLOCKS, &block_count);
    for(int i = 0; i != block_count; ++i) {
        block b{-1};
        int len;
        gl::glGetActiveUniformBlockName(id_, i, bufsize, &len, buffer);
        b.name = std::string{buffer, size_t(len)};
        gl::glGetActiveUniformBlockiv(id_, i, GL_UNIFORM_BLOCK_DATA_SIZE, &b.size);
        b.index = gl::glGetUniformBlockIndex(id_, buffer);
        LOG("Uniform Block:", b.index, LOG_BLUE, b.name, LOG_YELLOW, "size:", LOG_RED, b.size);
        uni_blocks.emplace_back(std::move(b));
    }

    gl_error_check();
    return uni_blocks;
}

void program::bind_uniform(int location, parameter_type type, int count, const char* data) {
    switch(type) {
        case parameter_type::PT_FLOAT: gl::glUniform1fv(location, count, reinterpret_cast<const float*>(data)); return;
        case parameter_type::PT_FLOAT_VEC2: gl::glUniform2fv(location, count, reinterpret_cast<const float*>(data)); return;
        case parameter_type::PT_FLOAT_VEC3: gl::glUniform3fv(location, count, reinterpret_cast<const float*>(data)); return;
        case parameter_type::PT_FLOAT_VEC4: gl::glUniform4fv(location, count, reinterpret_cast<const float*>(data)); return;
        case parameter_type::PT_INT: gl::glUniform1iv(location, count, reinterpret_cast<const int32_t*>(data)); return;
        case parameter_type::PT_INT_VEC2: gl::glUniform2iv(location, count, reinterpret_cast<const int32_t*>(data)); return;
        case parameter_type::PT_INT_VEC3: gl::glUniform3iv(location, count, reinterpret_cast<const int32_t*>(data)); return;
        case parameter_type::PT_INT_VEC4: gl::glUniform4iv(location, count, reinterpret_cast<const int32_t*>(data)); return;
        case parameter_type::PT_UINT: gl::glUniform1uiv(location, count, reinterpret_cast<const uint32_t*>(data)); return;
        case parameter_type::PT_UINT_VEC2: gl::glUniform2uiv(location, count, reinterpret_cast<const uint32_t*>(data)); return;
        case parameter_type::PT_UINT_VEC3: gl::glUniform3uiv(location, count, reinterpret_cast<const uint32_t*>(data)); return;
        case parameter_type::PT_UINT_VEC4: gl::glUniform4uiv(location, count, reinterpret_cast<const uint32_t*>(data)); return;
        case parameter_type::PT_BOOL: gl::glUniform1iv(location, count, reinterpret_cast<const int32_t*>(data)); return;
        case parameter_type::PT_BOOL_VEC2: gl::glUniform2iv(location, count, reinterpret_cast<const int32_t*>(data)); return;
        case parameter_type::PT_BOOL_VEC3: gl::glUniform3iv(location, count, reinterpret_cast<const int32_t*>(data)); return;
        case parameter_type::PT_BOOL_VEC4: gl::glUniform4iv(location, count, reinterpret_cast<const int32_t*>(data)); return;
        case parameter_type::PT_FLOAT_MAT2: gl::glUniformMatrix2fv(location, count, GL_FALSE, reinterpret_cast<const float*>(data)); return;
        case parameter_type::PT_FLOAT_MAT3: gl::glUniformMatrix3fv(location, count, GL_FALSE, reinterpret_cast<const float*>(data)); return;
        case parameter_type::PT_FLOAT_MAT4: gl::glUniformMatrix4fv(location, count, GL_FALSE, reinterpret_cast<const float*>(data)); return;
        case parameter_type::PT_SAMPLER_1D:
        case parameter_type::PT_SAMPLER_2D:
        case parameter_type::PT_SAMPLER_3D:
        case parameter_type::PT_SAMPLER_CUBE:
        case parameter_type::PT_SAMPLER_1D_SHADOW:
        case parameter_type::PT_SAMPLER_2D_SHADOW:
        case parameter_type::PT_SAMPLER_1D_ARRAY:
        case parameter_type::PT_SAMPLER_2D_ARRAY:
        case parameter_type::PT_SAMPLER_1D_ARRAY_SHADOW:
        case parameter_type::PT_SAMPLER_2D_ARRAY_SHADOW:
        case parameter_type::PT_SAMPLER_2D_MULTISAMPLE:
        case parameter_type::PT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case parameter_type::PT_SAMPLER_CUBE_SHADOW:
        case parameter_type::PT_SAMPLER_BUFFER:
        case parameter_type::PT_SAMPLER_2D_RECT:
        case parameter_type::PT_SAMPLER_2D_RECT_SHADOW:
        case parameter_type::PT_INT_SAMPLER_1D:
        case parameter_type::PT_INT_SAMPLER_2D:
        case parameter_type::PT_INT_SAMPLER_3D:
        case parameter_type::PT_INT_SAMPLER_CUBE:
        case parameter_type::PT_INT_SAMPLER_1D_ARRAY:
        case parameter_type::PT_INT_SAMPLER_2D_ARRAY:
        case parameter_type::PT_INT_SAMPLER_2D_MULTISAMPLE:
        case parameter_type::PT_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case parameter_type::PT_INT_SAMPLER_BUFFER:
        case parameter_type::PT_INT_SAMPLER_2D_RECT:
        case parameter_type::PT_UINT_SAMPLER_1D:
        case parameter_type::PT_UINT_SAMPLER_2D:
        case parameter_type::PT_UINT_SAMPLER_3D:
        case parameter_type::PT_UINT_SAMPLER_CUBE:
        case parameter_type::PT_UINT_SAMPLER_1D_ARRAY:
        case parameter_type::PT_UINT_SAMPLER_2D_ARRAY:
        case parameter_type::PT_UINT_SAMPLER_2D_MULTISAMPLE:
        case parameter_type::PT_UINT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case parameter_type::PT_UINT_SAMPLER_BUFFER:
        case parameter_type::PT_UINT_SAMPLER_2D_RECT:
                    gl::glUniform1iv(location, count, reinterpret_cast<const int32_t*>(data)); return;

        default: LOG_ERR("Tried to bind invalid uniform at location:", location);
    }
}

void program::bind_block(int index, int location) {
    gl::glUniformBlockBinding(id_, index, location);
}

template <> void zap::engine::program::bind_uniform<int>(int location, const int& value) {
    gl::glUniform1i(location, value);
}

template <> void zap::engine::program::bind_uniform<float>(int location, const float& value) {
    gl::glUniform1f(location, value);
}

template <> void zap::engine::program::bind_uniform<zap::maths::vec2f>(int location, const zap::maths::vec2f& type) {
    gl::glUniform2fv(location, 1, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::vec3f>(int location, const zap::maths::vec3f& type) {
    gl::glUniform3fv(location, 1, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::vec4f>(int location, const zap::maths::vec4f& type) {
    gl::glUniform4fv(location, 1, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::mat3f>(int location, const zap::maths::mat3f& type) {
    gl::glUniformMatrix3fv(location, 1, GL_FALSE, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::mat4f>(int location, const zap::maths::mat4f& type) {
    gl::glUniformMatrix4fv(location, 1, GL_FALSE, type.data());
}

template <> void program::bind_uniform<std::vector<int>>(int location, const std::vector<int>& type) {
    gl::glUniform1iv(location, uint32_t(type.size()), type.data());
}

template <> void program::bind_uniform<std::vector<zap::maths::vec2f>>(int location, const std::vector<zap::maths::vec2f>& type) {
    gl::glUniform2fv(location, uint32_t(type.size()), type.data()->data());
}

template <> void program::bind_uniform<std::vector<zap::maths::vec3f>>(int location, const std::vector<zap::maths::vec3f>& type) {
    gl::glUniform3fv(location, uint32_t(type.size()), type.data()->data());
}

template <> void program::bind_uniform<std::vector<zap::maths::vec4f>>(int location, const std::vector<zap::maths::vec4f>& type) {
    gl::glUniform4fv(location, uint32_t(type.size()), type.data()->data());
}

template <> void zap::engine::program::bind_uniform<std::vector<float>>(int location, const std::vector<float>& type) {
    gl::glUniform1fv(location, uint32_t(type.size()), type.data());
}

template <> void zap::engine::program::bind_uniform<float>(const char* name, const float& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid mat4f uniform specified");
    if(loc != -1) gl::glUniform1f(loc, type);
}

template <> void zap::engine::program::bind_uniform<zap::maths::vec2f>(const char* name, const zap::maths::vec2f& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid vec2f uniform specified");
    if(loc != -1) gl::glUniform2fv(loc, 1, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::vec3f>(const char* name, const zap::maths::vec3f& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid vec3f uniform specified");
    if(loc != -1) gl::glUniform3fv(loc, 1, type.data());
}

template <> void zap::engine::program::bind_uniform<zap::maths::vec4f>(const char* name, const zap::maths::vec4f& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid vec4f uniform specified");
    if(loc != -1) gl::glUniform4fv(loc, 1, type.data());
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

template <> void zap::engine::program::bind_uniform<std::vector<int>>(const char* name, const std::vector<int>& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid vector<int> uniform specified");
    if(loc != -1) gl::glUniform1iv(loc, uint32_t(type.size()), type.data());
}

template <> void zap::engine::program::bind_uniform<std::vector<zap::maths::vec2f>>(const char* name, const std::vector<zap::maths::vec2f>& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid vector<vec2f> uniform specified");
    if(loc != -1) gl::glUniform2fv(loc, uint32_t(type.size()), type.data()->data());
}

template <> void zap::engine::program::bind_uniform<std::vector<zap::maths::vec3f>>(const char* name, const std::vector<zap::maths::vec3f>& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid vector<vec3f> uniform specified");
    if(loc != -1) gl::glUniform3fv(loc, uint32_t(type.size()), type.data()->data());
}

template <> void zap::engine::program::bind_uniform<std::vector<zap::maths::vec4f>>(const char* name, const std::vector<zap::maths::vec4f>& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid vector<vec3f> uniform specified");
    if(loc != -1) gl::glUniform4fv(loc, uint32_t(type.size()), type.data()->data());
}

template <> void zap::engine::program::bind_uniform<int>(const char* name, const int& value) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid int uniform specified");
    if(loc != -1) gl::glUniform1i(loc, value);
}

template <> void zap::engine::program::bind_uniform<std::vector<float>>(const char* name, const std::vector<float>& type) {
    auto loc = uniform_location(name);
    assert(loc != -1 && "Invalid float array uniform specified");
    if(loc != -1) gl::glUniform1fv(loc, uint32_t(type.size()), type.data());
}
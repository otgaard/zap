/* Created by Darren Otgaar on 2016/03/22. http://www.github.com/otgaard/zap */
#ifndef ZAP_GL_API_HPP
#define ZAP_GL_API_HPP

#include "engine.hpp"
#include <maths/maths.hpp>

namespace zap { namespace engine { namespace gl {
#ifdef __APPLE__
#include <OpenGL/gl3.h>
#elif __linux__
#define GL3_PROTOTYPES 1
#include <GL/gl.h>
#endif //__APPLE__
    constexpr GLenum gl_shader_types[(int)shader_type::ST_SIZE] = {
            GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
    };
    constexpr const char* gl_shader_type_names[(int)shader_type::ST_SIZE] = {
            "GL_VERTEX_SHADER", "GL_GEOMETRY_SHADER", "GL_FRAGMENT_SHADER"
    };

    constexpr GLenum gl_buffer_types[(int)buffer_type::BT_SIZE] = {
            GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER,
            GL_PIXEL_PACK_BUFFER, GL_PIXEL_UNPACK_BUFFER, GL_TEXTURE_BUFFER, GL_TRANSFORM_FEEDBACK_BUFFER,
            GL_UNIFORM_BUFFER
    };
    constexpr const char* gl_buffer_type_names[(int)buffer_type::BT_SIZE] = {
            "GL_ARRAY_BUFFER", "GL_ELEMENT_ARRAY_BUFFER", "GL_COPY_READ_BUFFER", "GL_COPY_WRITE_BUFFER",
            "GL_PIXEL_PACK_BUFFER", "GL_PIXEL_UNPACK_BUFFER", "GL_TEXTURE_BUFFER", "GL_TRANSFORM_FEEDBACK_BUFFER",
            "GL_UNIFORM_BUFFER"
    };

    constexpr GLenum gl_buffer_usage[(int)buffer_usage::BU_SIZE] = {
            GL_STATIC_DRAW, GL_STATIC_READ, GL_STATIC_COPY,
            GL_DYNAMIC_DRAW, GL_DYNAMIC_READ, GL_DYNAMIC_COPY,
            GL_STREAM_DRAW, GL_STREAM_READ, GL_STREAM_COPY
    };
    constexpr const char* gl_buffer_usage_names[(int)buffer_usage::BU_SIZE] = {
            "GL_STATIC_DRAW", "GL_STATIC_READ", "GL_STATIC_COPY",
            "GL_DYNAMIC_DRAW", "GL_DYNAMIC_READ", "GL_DYNAMIC_COPY",
            "GL_STREAM_DRAW", "GL_STREAM_READ", "GL_STREAM_COPY"
    };

    constexpr GLbitfield gl_buffer_access[(int)buffer_access::BA_SIZE] = {
            GL_READ_ONLY,
            GL_WRITE_ONLY,
            GL_READ_WRITE
    };
    constexpr const char* gl_buffer_access_names[(int)buffer_access::BA_SIZE] = {
            "GL_READ_ONLY",
            "GL_WRITE_ONLY",
            "GL_READ_WRITE"
    };

    constexpr GLenum gl_data_type[(int)data_type::DT_SIZE] = {
            GL_FALSE,
            GL_UNSIGNED_BYTE,
            GL_BYTE,
            GL_UNSIGNED_SHORT,
            GL_SHORT,
            GL_UNSIGNED_INT,
            GL_INT,
            GL_HALF_FLOAT,
            GL_FIXED,
            GL_FLOAT,
            GL_DOUBLE
    };

    constexpr const char* gl_data_type_names[(int)data_type::DT_SIZE] = {
            "GL_FALSE",
            "GL_UNSIGNED_BYTE",
            "GL_BYTE",
            "GL_UNSIGNED_SHORT",
            "GL_SHORT",
            "GL_UNSIGNED_INT",
            "GL_INT",
            "GL_HALF_FLOAT",
            "GL_FIXED",
            "GL_FLOAT",
            "GL_DOUBLE"
    };

    constexpr GLenum gl_type(shader_type type) { return gl_shader_types[(int)type]; }
    constexpr const char* gl_typename(shader_type type) { return gl_shader_type_names[(int)type]; }
    constexpr GLenum gl_type(buffer_type type) { return gl_buffer_types[(int)type]; }
    constexpr const char* gl_typename(buffer_type type) { return gl_buffer_type_names[(int)type]; }
    constexpr GLenum gl_type(buffer_usage usage) { return gl_buffer_usage[(int)usage]; }
    constexpr const char* gl_typename(buffer_usage usage) { return gl_buffer_usage_names[(int)usage]; }
    constexpr GLenum gl_type(data_type usage) { return gl_data_type[(int)usage]; }
    constexpr const char* gl_typename(data_type usage) { return gl_data_type_names[(int)usage]; }
    constexpr GLbitfield gl_type(buffer_access access) { return gl_buffer_access[(int)access]; }
    constexpr const char* gl_typename(buffer_access access) { return gl_buffer_access_names[(int)access]; }

    inline const char* gl_version() { return (const char*)glGetString(GL_VERSION); }
}}}

#endif //ZAP_GL_API_HPP

/* Created by Darren Otgaar on 2016/03/22. http://www.github.com/otgaard/zap */
#ifndef ZAP_GL_API_HPP
#define ZAP_GL_API_HPP

#include "engine.hpp"
#include <algorithm>
#include <maths/maths.hpp>

namespace zap { namespace engine { namespace gl {
#ifdef __APPLE__
#include <GL/glew.h>
#elif __linux__
#include <GL/glew.h>
#include <GL/gl.h>
#elif _WIN32
//#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>
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
            GL_READ_WRITE,
            GL_MAP_READ_BIT,
            GL_MAP_WRITE_BIT,
            GL_MAP_PERSISTENT_BIT,
            GL_MAP_COHERENT_BIT,
            GL_MAP_INVALIDATE_RANGE_BIT,
            GL_MAP_INVALIDATE_BUFFER_BIT,
            GL_MAP_FLUSH_EXPLICIT_BIT,
            GL_MAP_UNSYNCHRONIZED_BIT
    };
    constexpr const char* gl_buffer_access_names[(int)buffer_access::BA_SIZE] = {
            "GL_READ_ONLY",
            "GL_WRITE_ONLY",
            "GL_READ_WRITE",
            "GL_MAP_READ_BIT",
            "GL_MAP_WRITE_BIT",
            "GL_MAP_PERSISTENT_BIT",
            "GL_MAP_COHERENT_BIT",
            "GL_MAP_INVALIDATE_RANGE_BIT",
            "GL_MAP_INVALIDATE_BUFFER_BIT",
            "GL_MAP_FLUSH_EXPLICIT_BIT",
            "GL_MAP_UNSYNCHRONIZED_BIT"
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

    constexpr GLenum gl_primitive_type[(int)primitive_type::PT_SIZE] = {
            0,
            GL_POINTS,
            GL_LINES,
            GL_LINE_STRIP,
            GL_LINE_LOOP,
            GL_TRIANGLES,
            GL_TRIANGLE_STRIP,
            GL_TRIANGLE_FAN
    };

    constexpr const char* gl_primitive_type_names[(int)primitive_type::PT_SIZE] = {
            "NONE",
            "GL_POINTS",
            "GL_LINES",
            "GL_LINE_STRIP",
            "GL_LINE_LOOP",
            "GL_TRIANGLES",
            "GL_TRIANGLE_STRIP",
            "GL_TRIANGLE_FAN"
    };

    constexpr GLenum gl_pixel_format_type[(int)pixel_format::PF_SIZE] = {
            0,
            GL_RED,
            GL_RG,
            GL_RGB,
            GL_BGR,
            GL_RGBA,
            GL_BGRA,
            GL_DEPTH_COMPONENT,
            GL_DEPTH_STENCIL,
            GL_RED_INTEGER,
            GL_RG_INTEGER,
            GL_RGB_INTEGER,
            GL_BGR_INTEGER,
            GL_RGBA_INTEGER,
            GL_BGRA_INTEGER
    };

    constexpr const char* gl_pixel_format_names[(int)pixel_format::PF_SIZE] = {
            "NONE",
            "GL_RED",
            "GL_RG",
            "GL_RGB",
            "GL_BGR",
            "GL_RGBA",
            "GL_BGRA",
            "GL_DEPTH_COMPONENT",
            "GL_DEPTH_STENCIL",
            "GL_RED_INTEGER",
            "GL_RG_INTEGER",
            "GL_RGB_INTEGER",
            "GL_BGR_INTEGER",
            "GL_RGBA_INTEGER",
            "GL_BGRA_INTEGER"
    };

    constexpr GLenum gl_pixel_dtype_type[(int)pixel_datatype::PD_SIZE] = {
            0,
            GL_UNSIGNED_BYTE_3_3_2,
            GL_UNSIGNED_BYTE,
            GL_FLOAT,
            GL_UNSIGNED_INT_24_8,
            GL_UNSIGNED_INT,
            GL_UNSIGNED_BYTE,
    };

    constexpr const char* gl_pixel_dtype_names[(int)pixel_datatype::PD_SIZE] = {
            "NONE",
            "GL_UNSIGNED_BYTE_3_3_2",
            "GL_UNSIGNED_BYTE",
            "GL_FLOAT",
            "GL_UNSIGNED_INT_24_8",
            "GL_UNSIGNED_INT",
            "GL_UNSIGNED_BYTE"
    };

    constexpr GLenum gl_texture_type[(int)texture_type::TT_SIZE] = {
            GL_NONE,
            GL_TEXTURE_1D,
            GL_TEXTURE_2D,
            GL_TEXTURE_3D,
            GL_TEXTURE_2D_MULTISAMPLE,
            GL_TEXTURE_CUBE_MAP,
            GL_TEXTURE_RECTANGLE,
            GL_TEXTURE_1D_ARRAY,
            GL_TEXTURE_2D_ARRAY
    };

    constexpr const char* gl_texture_type_names[(int)texture_type::TT_SIZE] = {
            "GL_NONE",
            "GL_TEXTURE_1D",
            "GL_TEXTURE_2D",
            "GL_TEXTURE_3D",
            "GL_TEXTURE_2D_MULTISAMPLE",
            "GL_TEXTURE_CUBE_MAP",
            "GL_TEXTURE_RECTANGLE",
            "GL_TEXTURE_1D_ARRAY",
            "GL_TEXTURE_2D_ARRAY"
    };

    constexpr GLenum gl_tex_parm[(int)tex_parm::TP_SIZE] = {
            GL_TEXTURE_WRAP_S,
            GL_TEXTURE_WRAP_T,
            GL_TEXTURE_WRAP_R,
            GL_TEXTURE_MAG_FILTER,
            GL_TEXTURE_MIN_FILTER,
            GL_TEXTURE_MAX_ANISOTROPY_EXT,
            GL_TEXTURE_BORDER_COLOR
    };

    constexpr const char* gl_tex_parm_names[(int)tex_parm::TP_SIZE] = {
            "GL_TEXTURE_WRAP_S",
            "GL_TEXTURE_WRAP_T",
            "GL_TEXTURE_WRAP_R",
            "GL_TEXTURE_MAG_FILTER",
            "GL_TEXTURE_MIN_FILTER",
            "GL_TEXTURE_MAX_ANISOTROPY_EXT",
            "GL_TEXTURE_BORDER_COLOR"
    };

    constexpr GLenum gl_tex_filter[(int)tex_filter::TF_SIZE] = {
            GL_NEAREST,
            GL_LINEAR,
            GL_NEAREST_MIPMAP_NEAREST,
            GL_LINEAR_MIPMAP_NEAREST,
            GL_NEAREST_MIPMAP_LINEAR,
            GL_LINEAR_MIPMAP_LINEAR
    };

    constexpr const char* gl_tex_filter_names[(int)tex_filter::TF_SIZE] = {
            "GL_NEAREST",
            "GL_LINEAR",
            "GL_NEAREST_MIPMAP_NEAREST",
            "GL_LINEAR_MIPMAP_NEAREST",
            "GL_NEAREST_MIPMAP_LINEAR",
            "GL_LINEAR_MIPMAP_LINEAR"
    };

    constexpr GLenum gl_tex_wrap[(int)tex_wrap::TW_SIZE] = {
            GL_CLAMP_TO_EDGE,
            GL_CLAMP_TO_BORDER,
            GL_MIRRORED_REPEAT,
            GL_REPEAT,
            GL_MIRROR_CLAMP_TO_EDGE
    };

    constexpr const char* gl_tex_wrap_names[(int)tex_wrap::TW_SIZE] = {
            "GL_CLAMP_TO_EDGE",
            "GL_CLAMP_TO_BORDER",
            "GL_MIRRORED_REPEAT",
            "GL_REPEAT",
            "GL_MIRROR_CLAMP_TO_EDGE"
    };

    constexpr GLenum gl_parameter_type[(int)parameter_type::PT_SIZE] = {
            0,
            GL_FLOAT,
            GL_FLOAT_VEC2,
            GL_FLOAT_VEC3,
            GL_FLOAT_VEC4,
            GL_INT,
            GL_INT_VEC2,
            GL_INT_VEC3,
            GL_INT_VEC4,
            GL_UNSIGNED_INT,
            GL_UNSIGNED_INT_VEC2,
            GL_UNSIGNED_INT_VEC3,
            GL_UNSIGNED_INT_VEC4,
            GL_BOOL,
            GL_BOOL_VEC2,
            GL_BOOL_VEC3,
            GL_BOOL_VEC4,
            GL_FLOAT_MAT2,
            GL_FLOAT_MAT3,
            GL_FLOAT_MAT4,
            GL_FLOAT_MAT2x3,
            GL_FLOAT_MAT2x4,
            GL_FLOAT_MAT3x2,
            GL_FLOAT_MAT3x4,
            GL_FLOAT_MAT4x2,
            GL_FLOAT_MAT4x3,
            GL_SAMPLER_1D,
            GL_SAMPLER_2D,
            GL_SAMPLER_3D,
            GL_SAMPLER_CUBE,
            GL_SAMPLER_1D_SHADOW,
            GL_SAMPLER_2D_SHADOW,
            GL_SAMPLER_1D_ARRAY,
            GL_SAMPLER_2D_ARRAY,
            GL_SAMPLER_1D_ARRAY_SHADOW,
            GL_SAMPLER_2D_ARRAY_SHADOW,
            GL_SAMPLER_2D_MULTISAMPLE,
            GL_SAMPLER_2D_MULTISAMPLE_ARRAY,
            GL_SAMPLER_CUBE_SHADOW,
            GL_SAMPLER_BUFFER,
            GL_SAMPLER_2D_RECT,
            GL_SAMPLER_2D_RECT_SHADOW,
            GL_INT_SAMPLER_1D,
            GL_INT_SAMPLER_2D,
            GL_INT_SAMPLER_3D,
            GL_INT_SAMPLER_CUBE,
            GL_INT_SAMPLER_1D_ARRAY,
            GL_INT_SAMPLER_2D_ARRAY,
            GL_INT_SAMPLER_2D_MULTISAMPLE,
            GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
            GL_INT_SAMPLER_BUFFER,
            GL_INT_SAMPLER_2D_RECT,
            GL_UNSIGNED_INT_SAMPLER_1D,
            GL_UNSIGNED_INT_SAMPLER_2D,
            GL_UNSIGNED_INT_SAMPLER_3D,
            GL_UNSIGNED_INT_SAMPLER_CUBE,
            GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,
            GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
            GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,
            GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
            GL_UNSIGNED_INT_SAMPLER_BUFFER,
            GL_UNSIGNED_INT_SAMPLER_2D_RECT
    };

    constexpr const char* gl_parameter_type_names[(int)parameter_type::PT_SIZE] = {
            "VOID",
            "GL_FLOAT",
            "GL_FLOAT_VEC2",
            "GL_FLOAT_VEC3",
            "GL_FLOAT_VEC4",
            "GL_INT",
            "GL_INT_VEC2",
            "GL_INT_VEC3",
            "GL_INT_VEC4",
            "GL_UNSIGNED_INT",
            "GL_UNSIGNED_INT_VEC2",
            "GL_UNSIGNED_INT_VEC3",
            "GL_UNSIGNED_INT_VEC4",
            "GL_BOOL",
            "GL_BOOL_VEC2",
            "GL_BOOL_VEC3",
            "GL_BOOL_VEC4",
            "GL_FLOAT_MAT2",
            "GL_FLOAT_MAT3",
            "GL_FLOAT_MAT4",
            "GL_FLOAT_MAT2x3",
            "GL_FLOAT_MAT2x4",
            "GL_FLOAT_MAT3x2",
            "GL_FLOAT_MAT3x4",
            "GL_FLOAT_MAT4x2",
            "GL_FLOAT_MAT4x3",
            "GL_SAMPLER_1D",
            "GL_SAMPLER_2D",
            "GL_SAMPLER_3D",
            "GL_SAMPLER_CUBE",
            "GL_SAMPLER_1D_SHADOW",
            "GL_SAMPLER_2D_SHADOW",
            "GL_SAMPLER_1D_ARRAY",
            "GL_SAMPLER_2D_ARRAY",
            "GL_SAMPLER_1D_ARRAY_SHADOW",
            "GL_SAMPLER_2D_ARRAY_SHADOW",
            "GL_SAMPLER_2D_MULTISAMPLE",
            "GL_SAMPLER_2D_MULTISAMPLE_ARRAY",
            "GL_SAMPLER_CUBE_SHADOW",
            "GL_SAMPLER_BUFFER",
            "GL_SAMPLER_2D_RECT",
            "GL_SAMPLER_2D_RECT_SHADOW",
            "GL_INT_SAMPLER_1D",
            "GL_INT_SAMPLER_2D",
            "GL_INT_SAMPLER_3D",
            "GL_INT_SAMPLER_CUBE",
            "GL_INT_SAMPLER_1D_ARRAY",
            "GL_INT_SAMPLER_2D_ARRAY",
            "GL_INT_SAMPLER_2D_MULTISAMPLE",
            "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY",
            "GL_INT_SAMPLER_BUFFER",
            "GL_INT_SAMPLER_2D_RECT",
            "GL_UNSIGNED_INT_SAMPLER_1D",
            "GL_UNSIGNED_INT_SAMPLER_2D",
            "GL_UNSIGNED_INT_SAMPLER_3D",
            "GL_UNSIGNED_INT_SAMPLER_CUBE",
            "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY",
            "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY",
            "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE",
            "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY",
            "GL_UNSIGNED_INT_SAMPLER_BUFFER",
            "GL_UNSIGNED_INT_SAMPLER_2D_RECT"
    };

    template <typename EngineType>
    bool internal_type(GLenum gltype, const GLenum* begin, const GLenum* end, EngineType& type) {
        auto it = std::find(begin, end, gltype);
        if(it == end) return false;
        type = (EngineType)(it - begin);
        return true;
    }

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
    constexpr GLenum gl_type(primitive_type type) { return gl_primitive_type[(int)type]; }
    constexpr const char* gl_typename(primitive_type type) { return gl_primitive_type_names[(int)type]; }
    constexpr GLenum gl_type(pixel_format type) { return gl_pixel_format_type[(int)type]; }
    constexpr const char* gl_typename(pixel_format type) { return gl_pixel_format_names[(int)type]; }
    constexpr GLenum gl_type(pixel_datatype type) { return gl_pixel_dtype_type[(int)type]; }
    constexpr const char* gl_typename(pixel_datatype type) { return gl_pixel_dtype_names[(int)type]; }
    constexpr GLenum gl_type(texture_type type) { return gl_texture_type[(int)type]; }
    constexpr const char* gl_typename(texture_type type) { return gl_texture_type_names[(int)type]; }
    constexpr GLenum gl_type(tex_parm type) { return gl_tex_parm[(int)type]; }
    constexpr const char* gl_typename(tex_parm type) { return gl_tex_parm_names[(int)type]; }
    constexpr GLenum gl_type(tex_filter type) { return gl_tex_filter[(int)type]; }
    constexpr const char* gl_typename(tex_filter type) { return gl_tex_filter_names[(int)type]; }
    constexpr GLenum gl_type(tex_wrap type) { return gl_tex_wrap[(int)type]; }
    constexpr const char* gl_typename(tex_wrap type) { return gl_tex_wrap_names[(int)type]; }
    constexpr GLenum gl_type(parameter_type type) { return gl_parameter_type[(int)type]; }
    constexpr const char* gl_typename(parameter_type type) { return gl_parameter_type_names[(int)type]; }

    inline const char* gl_version() { return (const char*)glGetString(GL_VERSION); }

    void bind_buffer_base(buffer_type type, int idx, uint32_t bo);

}}}

#endif //ZAP_GL_API_HPP

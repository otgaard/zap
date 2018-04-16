//
// Created by Darren Otgaar on 2016/03/22.
//

#ifndef ZAP_ENGINE_HPP
#define ZAP_ENGINE_HPP

#if defined(_WIN32)

#if !defined(ENGINE_EXPORT)
#if defined(ZAP_STATIC)
#include "engine_exports_s.h"
#else
#include "engine_exports.h"
#endif
#else
#include ENGINE_EXPORT
#endif

#else
#define ZAPENGINE_EXPORT
#endif

#include <core/core.hpp>
#include <maths/maths.hpp>
#include <memory>
#include <cassert>
#include <cstdint>
#include <maths/algebra.hpp>
#include "tools/log.hpp"

namespace zap { namespace engine {
    using resource_t = std::uint32_t;
    constexpr resource_t INVALID_RESOURCE = resource_t(-1);

    // Engine Definitions and Enumerations
    enum class shader_type : std::uint8_t {
        ST_VERTEX,
        ST_GEOMETRY,
        ST_FRAGMENT,
        ST_SIZE
    };

    enum class buffer_type : std::uint8_t {
        BT_ARRAY,
        BT_ELEMENT_ARRAY,
        BT_COPY_READ,
        BT_COPY_WRITE,
        BT_PIXEL_PACK,
        BT_PIXEL_UNPACK,
        BT_TEXTURE,
        BT_TRANSFORM,
        BT_UNIFORM,
        BT_SIZE
    };

    enum class buffer_usage : std::uint8_t {
        BU_STATIC_DRAW,
        BU_STATIC_READ,
        BU_STATIC_COPY,
        BU_DYNAMIC_DRAW,
        BU_DYNAMIC_READ,
        BU_DYNAMIC_COPY,
        BU_STREAM_DRAW,
        BU_STREAM_READ,
        BU_STREAM_COPY,
        BU_SIZE
    };

    enum class buffer_access : std::uint8_t {
        BA_READ_ONLY,
        BA_WRITE_ONLY,
        BA_READ_WRITE,
        BA_SIZE
    };

    struct range_access {
        enum code {
            BA_MAP_READ = 1 << 0,
            BA_MAP_WRITE = 1 << 1,
            BA_MAP_PERSISTENT = 1 << 2,
            BA_MAP_COHERENT = 1 << 3,
            BA_MAP_INVALIDATE_RANGE = 1 << 4,
            BA_MAP_INVALIDATE_BUFFER = 1 << 5,
            BA_MAP_FLUSH_EXPLICIT = 1 << 6,
            BA_MAP_UNSYNCHRONISED = 1 << 7,
            BA_SIZE = 1 << 8
        };
    };

    enum class data_type : std::uint8_t {
        DT_VOID = 0,
        DT_UBYTE = 1,
        DT_BYTE = 2,
        DT_USHORT = 3,
        DT_SHORT = 4,
        DT_UINT = 5,
        DT_INT = 6,
        DT_HALF_FLOAT = 7,
        DT_FIXED = 8,
        DT_FLOAT = 9,
        DT_DOUBLE = 10,
        DT_SIZE = 11
    };

    enum class pixel_format : size_t {
        PF_NONE = 0,
        PF_RED,
        PF_RG,
        PF_RGB,
        PF_BGR,
        PF_RGBA,
        PF_BGRA,
        PF_DEPTH_COMPONENT,
        PF_DEPTH_STENCIL,
        PF_RED_INTEGER,
        PF_RG_INTEGER,
        PF_RGB_INTEGER,
        PF_BGR_INTEGER,
        PF_RGBA_INTEGER,
        PF_BGRA_INTEGER,
        PF_SIZE
    };

    enum class pixel_datatype : size_t {
        PD_NONE = 0,
        PD_UNSIGNED_BYTE_3_3_2,
        PD_UNSIGNED_BYTE,
        PD_FLOAT,
        PD_UNSIGNED_INT_24_8,
        PD_UNSIGNED_INT,
        PD_DN_UNSIGNED_BYTE, // denormalised unsigned byte
        PD_SIZE
    };

    template <typename T> struct dt_descriptor { enum { value = int(data_type::DT_VOID) }; };
    template <> struct dt_descriptor<unsigned char> { enum { value = int(data_type::DT_UBYTE) }; };
    template <> struct dt_descriptor<char> { enum { value = int(data_type::DT_BYTE) }; };
    template <> struct dt_descriptor<unsigned short> { enum { value = int(data_type::DT_USHORT) }; };
    template <> struct dt_descriptor<short> { enum { value = int(data_type::DT_SHORT) }; };
    template <> struct dt_descriptor<unsigned int> { enum { value = int(data_type::DT_UINT) }; };
    template <> struct dt_descriptor<int> { enum { value = int(data_type::DT_INT) }; };
    template <> struct dt_descriptor<float> { enum { value = int(data_type::DT_FLOAT) }; };
    template <> struct dt_descriptor<double> { enum { value = int(data_type::DT_DOUBLE) }; };

    template <data_type DT> struct dt_type { using type = void; };
    template <> struct dt_type<data_type::DT_UBYTE> { using type = unsigned char;  };
    template <> struct dt_type<data_type::DT_BYTE> { using type = char; };
    template <> struct dt_type<data_type::DT_USHORT> { using type = unsigned short; };
    template <> struct dt_type<data_type::DT_SHORT> { using type = short; };
    template <> struct dt_type<data_type::DT_UINT> { using type = unsigned int; };
    template <> struct dt_type<data_type::DT_INT> { using type = int; };
    template <> struct dt_type<data_type::DT_HALF_FLOAT> { using type = void; };
    template <> struct dt_type<data_type::DT_FIXED> { using type = void; };
    template <> struct dt_type<data_type::DT_FLOAT> { using type = float; };
    template <> struct dt_type<data_type::DT_DOUBLE> { using type = double; };
    template <> struct dt_type<data_type::DT_SIZE> { using type = void; };

    inline size_t dt_bytesize(data_type dt) {
        switch(dt) {
            case data_type::DT_VOID: return 0;
            case data_type::DT_UBYTE: return sizeof(typename dt_type<data_type::DT_UBYTE>::type);
            case data_type::DT_BYTE: return sizeof(typename dt_type<data_type::DT_BYTE>::type);
            case data_type::DT_USHORT: return sizeof(typename dt_type<data_type::DT_USHORT>::type);
            case data_type::DT_SHORT: return sizeof(typename dt_type<data_type::DT_SHORT>::type);
            case data_type::DT_UINT: return sizeof(typename dt_type<data_type::DT_UINT>::type);
            case data_type::DT_INT: return sizeof(typename dt_type<data_type::DT_INT>::type);
            case data_type::DT_HALF_FLOAT: return 0;
            case data_type::DT_FIXED: return 0;
            case data_type::DT_FLOAT: return sizeof(dt_type<data_type::DT_FLOAT>::type);
            case data_type::DT_DOUBLE: return sizeof(dt_type<data_type::DT_DOUBLE>::type);
            case data_type::DT_SIZE: return 0;
			default: return 0;
        }
    }

    enum class attribute_type : size_t {
        AT_POSITION,
        AT_NORMAL,
        AT_TANGENT,
        AT_BITANGENT,
        AT_TEXCOORD1,
        AT_TEXCOORD2,
        AT_TEXCOORD3,
        AT_TEXCOORD4,
        AT_TEXCOORD5,
        AT_TEXCOORD6,
        AT_COLOUR1,
        AT_COLOUR2,
        AT_BLENDINDEX,
        AT_BLENDWEIGHT,
        AT_FOGCOORD,
        AT_POINTSIZE,
        AT_SIZE
    };

    constexpr static const char* const attribute_name[size_t(attribute_type::AT_SIZE)] = {
        "position",
        "normal",
        "tangent",
        "bitangent",
        "texcoord1",
        "texcoord2",
        "texcoord3",
        "texcoord4",
        "texcoord5",
        "texcoord6",
        "colour1",
        "colour2",
        "blendindex",
        "blendweight",
        "fogcoord",
        "pointsize"
    };

    enum class generic_attribute_type : size_t {
        AT_LOCATION_0,
        AT_LOCATION_1,
        AT_LOCATION_2,
        AT_LOCATION_3,
        AT_LOCATION_4,
        AT_LOCATION_5,
        AT_LOCATION_6,
        AT_LOCATION_7,
        AT_LOCATION_8,
        AT_LOCATION_9,
        AT_LOCATION_10,
        AT_LOCATION_11,
        AT_LOCATION_12,
        AT_LOCATION_13,
        AT_LOCATION_14,
        AT_LOCATION_15,
        AT_SIZE
    };

    constexpr static const char* const generic_attribute_name[size_t(attribute_type::AT_SIZE)] = {
        "loc0",
        "loc1",
        "loc2",
        "loc3",
        "loc4",
        "loc5",
        "loc6",
        "loc7",
        "loc8",
        "loc9",
        "loc10",
        "loc11",
        "loc12",
        "loc13",
        "loc14",
        "loc15"
    };

    enum class primitive_type : std::uint8_t {
        PT_NONE = 0,
        PT_POINTS = 1,
        PT_LINES = 2,
        PT_LINE_STRIP = 3,
        PT_LINE_LOOP = 4,
        PT_TRIANGLES = 5,
        PT_TRIANGLE_STRIP = 6,
        PT_TRIANGLE_FAN = 7,
        PT_SIZE = 8
    };

    enum class texture_type : std::uint8_t {
        TT_NONE = 0,
        TT_TEX1D = 1,
        TT_TEX2D = 2,
        TT_TEX3D = 3,
        TT_TEX2D_MS = 4,
        TT_CUBE_MAP = 5,
        TT_RECTANGLE = 6,
        TT_TEX1D_ARR = 7,
        TT_TEX2D_ARR = 8,
        TT_SIZE = 9
    };

    enum class tex_parm : std::uint8_t {
        TP_WRAP_S = 0,
        TP_WRAP_T = 1,
        TP_WRAP_R = 2,
        TP_MAG_FILTER = 3,
        TP_MIN_FILTER = 4,
        TP_MAX_ANISOTROPY = 5,
        TP_BORDER_COLOUR = 6,
        TP_SIZE = 7
    };

    enum class tex_filter : std::uint8_t {
        TF_NEAREST = 0,
        TF_LINEAR = 1,
        TF_NEAREST_MIPMAP_NEAREST = 2,
        TF_LINEAR_MIPMAP_NEAREST = 3,
        TF_NEAREST_MIPMAP_LINEAR = 4,
        TF_LINEAR_MIPMAP_LINEAR = 5,
        TF_SIZE = 6
    };

    enum class tex_wrap : std::uint8_t {
        TW_CLAMP_TO_EDGE = 0,
        TW_CLAMP_TO_BORDER = 1,
        TW_MIRRORED_REPEAT = 2,
        TW_REPEAT = 3,
        TW_MIRROR_CLAMP_TO_EDGE = 4,
        TW_SIZE = 5
    };

    enum class parameter_type {
        PT_VOID,
        PT_FLOAT,
        PT_FLOAT_VEC2,
        PT_FLOAT_VEC3,
        PT_FLOAT_VEC4,
        PT_INT,
        PT_INT_VEC2,
        PT_INT_VEC3,
        PT_INT_VEC4,
        PT_UINT,
        PT_UINT_VEC2,
        PT_UINT_VEC3,
        PT_UINT_VEC4,
        PT_BOOL,
        PT_BOOL_VEC2,
        PT_BOOL_VEC3,
        PT_BOOL_VEC4,
        PT_FLOAT_MAT2,
        PT_FLOAT_MAT3,
        PT_FLOAT_MAT4,
        PT_FLOAT_MAT2x3,
        PT_FLOAT_MAT2x4,
        PT_FLOAT_MAT3x2,
        PT_FLOAT_MAT3x4,
        PT_FLOAT_MAT4x2,
        PT_FLOAT_MAT4x3,
        PT_SAMPLER_1D,
        PT_SAMPLER_2D,
        PT_SAMPLER_3D,
        PT_SAMPLER_CUBE,
        PT_SAMPLER_1D_SHADOW,
        PT_SAMPLER_2D_SHADOW,
        PT_SAMPLER_1D_ARRAY,
        PT_SAMPLER_2D_ARRAY,
        PT_SAMPLER_1D_ARRAY_SHADOW,
        PT_SAMPLER_2D_ARRAY_SHADOW,
        PT_SAMPLER_2D_MULTISAMPLE,
        PT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        PT_SAMPLER_CUBE_SHADOW,
        PT_SAMPLER_BUFFER,
        PT_SAMPLER_2D_RECT,
        PT_SAMPLER_2D_RECT_SHADOW,
        PT_INT_SAMPLER_1D,
        PT_INT_SAMPLER_2D,
        PT_INT_SAMPLER_3D,
        PT_INT_SAMPLER_CUBE,
        PT_INT_SAMPLER_1D_ARRAY,
        PT_INT_SAMPLER_2D_ARRAY,
        PT_INT_SAMPLER_2D_MULTISAMPLE,
        PT_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        PT_INT_SAMPLER_BUFFER,
        PT_INT_SAMPLER_2D_RECT,
        PT_UINT_SAMPLER_1D,
        PT_UINT_SAMPLER_2D,
        PT_UINT_SAMPLER_3D,
        PT_UINT_SAMPLER_CUBE,
        PT_UINT_SAMPLER_1D_ARRAY,
        PT_UINT_SAMPLER_2D_ARRAY,
        PT_UINT_SAMPLER_2D_MULTISAMPLE,
        PT_UINT_SAMPLER_2D_MULTISAMPLE_ARRAY,
        PT_UINT_SAMPLER_BUFFER,
        PT_UINT_SAMPLER_2D_RECT,
        PT_SIZE
    };

    // TODO:  Complete these tables

    template <typename T> struct pt_descriptor { enum { value = int(parameter_type::PT_VOID) }; };
    template <> struct pt_descriptor<float> { enum { value = int(parameter_type::PT_FLOAT) }; };
    template <> struct pt_descriptor<maths::vec2f> { enum { value = int(parameter_type::PT_FLOAT_VEC2) }; };
    template <> struct pt_descriptor<maths::vec3f> { enum { value = int(parameter_type::PT_FLOAT_VEC3) }; };
    template <> struct pt_descriptor<maths::vec4f> { enum { value = int(parameter_type::PT_FLOAT_VEC4) }; };
    template <> struct pt_descriptor<int> { enum { value = int(parameter_type::PT_INT) }; };
    template <> struct pt_descriptor<maths::vec2i> { enum { value = int(parameter_type::PT_INT_VEC2) }; };
    template <> struct pt_descriptor<maths::vec3i> { enum { value = int(parameter_type::PT_INT_VEC3) }; };
    template <> struct pt_descriptor<maths::vec4i> { enum { value = int(parameter_type::PT_INT_VEC4) }; };
    template <> struct pt_descriptor<uint32_t> { enum { value = int(parameter_type::PT_UINT) }; };
    template <> struct pt_descriptor<maths::vec2u> { enum { value = int(parameter_type::PT_UINT_VEC2) }; };
    template <> struct pt_descriptor<maths::vec3u> { enum { value = int(parameter_type::PT_UINT_VEC3) }; };
    template <> struct pt_descriptor<maths::vec4u> { enum { value = int(parameter_type::PT_UINT_VEC4) }; };
    template <> struct pt_descriptor<uint8_t> { enum { value = int(parameter_type::PT_BOOL) }; };
    template <> struct pt_descriptor<maths::vec2b> { enum { value = int(parameter_type::PT_BOOL_VEC2) }; };
    template <> struct pt_descriptor<maths::vec3b> { enum { value = int(parameter_type::PT_BOOL_VEC3) }; };
    template <> struct pt_descriptor<maths::vec4b> { enum { value = int(parameter_type::PT_BOOL_VEC4) }; };
    template <> struct pt_descriptor<maths::mat2f> { enum { value = int(parameter_type::PT_FLOAT_MAT2) }; };
    template <> struct pt_descriptor<maths::mat3f> { enum { value = int(parameter_type::PT_FLOAT_MAT3) }; };
    template <> struct pt_descriptor<maths::mat4f> { enum { value = int(parameter_type::PT_FLOAT_MAT4) }; };

    template <parameter_type PT> struct pt_type { using type = void; };
    template <> struct pt_type<parameter_type::PT_FLOAT> { using type = float; };
    template <> struct pt_type<parameter_type::PT_FLOAT_VEC2> { using type = maths::vec2f; };
    template <> struct pt_type<parameter_type::PT_FLOAT_VEC3> { using type = maths::vec3f; };
    template <> struct pt_type<parameter_type::PT_FLOAT_VEC4> { using type = maths::vec4f; };
    template <> struct pt_type<parameter_type::PT_INT> { using type = int32_t; };
    template <> struct pt_type<parameter_type::PT_INT_VEC2> { using type = maths::vec2i; };
    template <> struct pt_type<parameter_type::PT_INT_VEC3> { using type = maths::vec3i; };
    template <> struct pt_type<parameter_type::PT_INT_VEC4> { using type = maths::vec4i; };
    template <> struct pt_type<parameter_type::PT_UINT> { using type = uint32_t; };
    template <> struct pt_type<parameter_type::PT_UINT_VEC2> { using type = maths::vec2u; };
    template <> struct pt_type<parameter_type::PT_UINT_VEC3> { using type = maths::vec3u; };
    template <> struct pt_type<parameter_type::PT_UINT_VEC4> { using type = maths::vec4u; };
    template <> struct pt_type<parameter_type::PT_BOOL> { using type = uint8_t; };
    template <> struct pt_type<parameter_type::PT_BOOL_VEC2> { using type = maths::vec2b; };
    template <> struct pt_type<parameter_type::PT_BOOL_VEC3> { using type = maths::vec3b; };
    template <> struct pt_type<parameter_type::PT_BOOL_VEC4> { using type = maths::vec4b; };
    template <> struct pt_type<parameter_type::PT_FLOAT_MAT2> { using type = maths::mat2f; };
    template <> struct pt_type<parameter_type::PT_FLOAT_MAT3> { using type = maths::mat3f; };
    template <> struct pt_type<parameter_type::PT_FLOAT_MAT4> { using type = maths::mat4f; };

    constexpr size_t pt_bytesize[int(parameter_type::PT_SIZE)] = {
            0,
            sizeof(float),
            sizeof(maths::vec2f),
            sizeof(maths::vec3f),
            sizeof(maths::vec4f),
            sizeof(int32_t),
            sizeof(maths::vec2i),
            sizeof(maths::vec3i),
            sizeof(maths::vec4i),
            sizeof(uint32_t),
            sizeof(maths::vec2u),
            sizeof(maths::vec3u),
            sizeof(maths::vec4u),
            sizeof(uint8_t),
            sizeof(maths::vec2b),
            sizeof(maths::vec3b),
            sizeof(maths::vec4b),
            sizeof(maths::mat2f),
            sizeof(maths::mat3f),
            sizeof(maths::mat4f),
            0,  // GL_FLOAT_MAT2x3 etc...
            0,
            0,
            0,
            0,
            0,
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t),
            sizeof(int32_t)
    };

    ZAPENGINE_EXPORT bool init();

    ZAPENGINE_EXPORT bool _gl_error_log(const char* file, int line);
    ZAPENGINE_EXPORT bool _gl_error_check();

#ifdef LOGGING_ENABLED
#define gl_error_check() _gl_error_log(__FILE__, __LINE__)
#else
#define gl_error_check() _gl_error_check()
#endif
}}

#endif //ZAP_ENGINE_HPP

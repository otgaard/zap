//
// Created by Darren Otgaar on 2016/03/22.
//

#ifndef ZAP_ENGINE_HPP
#define ZAP_ENGINE_HPP

#include <core/core.hpp>
#include <maths/maths.hpp>

#include <memory>
#include <cassert>
#include <cstdint>

#define LOGGING_ENABLED
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
    template <> struct dt_type<data_type::DT_UBYTE> { using type = unsigned char; };
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
            case data_type::DT_UBYTE: return sizeof(dt_type<data_type::DT_UBYTE>);
            case data_type::DT_BYTE: return sizeof(dt_type<data_type::DT_BYTE>);
            case data_type::DT_USHORT: return sizeof(dt_type<data_type::DT_USHORT>);
            case data_type::DT_SHORT: return sizeof(dt_type<data_type::DT_SHORT>);
            case data_type::DT_UINT: return sizeof(dt_type<data_type::DT_UINT>);
            case data_type::DT_INT: return sizeof(dt_type<data_type::DT_INT>);
            case data_type::DT_HALF_FLOAT: return sizeof(dt_type<data_type::DT_HALF_FLOAT>);
            case data_type::DT_FIXED: return sizeof(dt_type<data_type::DT_FIXED>);
            case data_type::DT_FLOAT: return sizeof(dt_type<data_type::DT_FLOAT>);
            case data_type::DT_DOUBLE: return sizeof(dt_type<data_type::DT_DOUBLE>);
            case data_type::DT_SIZE: return sizeof(dt_type<data_type::DT_SIZE>);
        }
    }
    enum class attribute_type : size_t {
        AT_POSITION = 1 << 0,
        AT_NORMAL = 1 << 1,
        AT_TANGENT = 1 << 2,
        AT_BINORMAL = 1 << 3,
        AT_TEXCOORD1 = 1 << 4,
        AT_TEXCOORD2 = 1 << 5,
        AT_TEXCOORD3 = 1 << 6,
        AT_TEXCOORD4 = 1 << 7,
        AT_TEXCOORD5 = 1 << 8,
        AT_TEXCOORD6 = 1 << 9,
        AT_COLOUR1 = 1 << 10,
        AT_COLOUR2 = 1 << 11,
        AT_BLENDINDEX = 1 << 12,
        AT_BLENDWEIGHT = 1 << 13,
        AT_FOGCOORD = 1 << 14,
        AT_POINTSIZE = 1 << 15,
        AT_SIZE = zap::maths::log2_pow2(1 << 16)
    };

    constexpr static const char* const attribute_name[size_t(attribute_type::AT_SIZE)] = {
            "position",
            "normal",
            "tangent",
            "binormal",
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

    bool init();

    bool _gl_error_log(const char* file, int line);
    bool _gl_error_check();

#ifdef LOGGING_ENABLED
#define gl_error_check() _gl_error_log(__FILE__, __LINE__)
#else
#define gl_error_check() _gl_error_check()
#endif
}}

#endif //ZAP_ENGINE_HPP

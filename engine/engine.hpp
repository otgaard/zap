//
// Created by Darren Otgaar on 2016/03/22.
//

#ifndef ZAP_ENGINE_HPP
#define ZAP_ENGINE_HPP

#include <core/core.hpp>

#include <memory>
#include <cassert>
#include <cstdint>

#define LOGGING_ENABLED
#include "tools/log.hpp"

namespace zap { namespace engine {
    using uchar = unsigned char;
    using byte = unsigned char;
    using uint = unsigned int;
    using ulong = unsigned long;

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

    struct buffer_access {
        constexpr static size_t BA_SIZE = 6;
        enum bitfield {
            BA_READ = 1 << 0,
            BA_WRITE = 1 << 1,
            BA_INVALIDATE_RANGE = 1 << 2,
            BA_INVALIDATE_BUFFER = 1 << 3,
            BA_FLUSH_EXPLICIT = 1 << 4,
            BA_UNSYNCHRONISED = 1 << 5
        };
    };

    enum class data_type : std::uint8_t {
        DT_VOID = 0,
        DT_UCHAR = 1,
        Dt_CHAR = 2,
        DT_USHORT = 3,
        DT_SHORT = 4,
        DT_UINT = 5,
        DT_INT = 6,
        DT_ULONG = 7,
        DT_LONG = 8,
        DT_FLOAT = 9,
        DT_DOUBLE = 10,
        DT_SIZE = 11
    };

    struct attribute_type {
        constexpr static size_t AT_SIZE = 16;
        enum bitfield {
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
            AT_POINTSIZE = 1 << 15
        };
    };

    bool _gl_error_log(const char* file, int line);
    bool _gl_error_check();

#ifdef LOGGING_ENABLED
#define gl_error_check() _gl_error_log(__FILE__, __LINE__)
#else
#define gl_error_check() _gl_error_check()
#endif
}}

#endif //ZAP_ENGINE_HPP

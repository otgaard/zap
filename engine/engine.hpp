//
// Created by Darren Otgaar on 2016/03/22.
//

#ifndef ZAP_ENGINE_HPP
#define ZAP_ENGINE_HPP

#include <memory>
#include <cassert>
#include <cstdint>

#define LOGGING_ENABLED

#include "tools/log.hpp"
// Load OpenGL Definitions in into gl namespace
/*
namespace zap { namespace engine { namespace gl {
#include <OpenGL/gl3.h>
}}}
*/

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

    enum class buffer_access : std::uint8_t {
        BA_READ = 1 << 0,
        BA_WRITE = 1 << 1,
        BA_INVALIDATE_RANGE = 1 << 2,
        BA_INVALIDATE_BUFFER = 1 << 3,
        BA_FLUSH_EXPLICIT = 1 << 4,
        BA_UNSYNCHRONISED = 1 << 5
    };

    bool _gl_error_log(const char* file, int line);
    bool _gl_error_check();

#ifdef LOGGING_ENABLED
#define gl_error_check() _gl_error_log(__FILE__, __LINE__)
#else
#define gl_error_check() _gl_error_check()
#endif
}}

#include "gl_api.hpp"

#endif //ZAP_ENGINE_HPP

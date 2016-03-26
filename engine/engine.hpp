//
// Created by Darren Otgaar on 2016/03/22.
//

#ifndef ZAP_ENGINE_HPP
#define ZAP_ENGINE_HPP

#include <memory>
#include <cassert>
#include <cstdint>

#include "tools/log.hpp"

// Load OpenGL Definitions in into API namespace
namespace zap { namespace engine { namespace gl {
#include <OpenGL/gl3.h>
}}}

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

    void _gl_error_log(const char* file, int line);

#define gl_error_log() _gl_error_log(__FILE__, __LINE__);
}}

#endif //ZAP_ENGINE_HPP

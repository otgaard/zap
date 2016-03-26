/* Created by Darren Otgaar on 2016/03/22. http://www.github.com/otgaard/zap */
#ifndef ZAP_GL_API_HPP
#define ZAP_GL_API_HPP

#include "engine.hpp"

namespace zap { namespace engine { namespace gl {
    constexpr GLenum gl_shader_types[(int)shader_type::ST_SIZE] = {
            GL_VERTEX_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER
    };
    constexpr const char* gl_shader_type_names[(int)shader_type::ST_SIZE] = {
            "GL_VERTEX_SHADER", "GL_GEOMETRY_SHADER", "GL_FRAGMENT_SHADER"
    };

    constexpr GLenum gl_type(shader_type type) { return gl_shader_types[(int)type]; }
    constexpr const char* gl_typename(shader_type type) { return gl_shader_type_names[(int)type]; }

    inline const char* gl_version() { return (const char*)glGetString(GL_VERSION); }
}}}

#endif //ZAP_GL_API_HPP

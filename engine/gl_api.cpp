/* Created by Darren Otgaar on 2016/03/22. http://www.github.com/otgaard/zap */
#include "gl_api.hpp"

using namespace zap::engine;

void zap::engine::_gl_error_log(const char* file, int line) {
    using namespace gl;

    GLenum err = glGetError();
    if(err != GL_NO_ERROR) {
        std::string error;
        switch(err) {
            case GL_INVALID_OPERATION: error = "GL_INVALID_OPERATION"; break;
            case GL_INVALID_ENUM: error = "GL_INVALID_ENUM"; break;
            case GL_INVALID_VALUE: error = "GL_INVALID_VALUE"; break;
            case GL_OUT_OF_MEMORY: error = "GL_OUT_OF_MEMORY"; break;
            case GL_INVALID_FRAMEBUFFER_OPERATION: error = "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        LOG_ERR("OpenGL Error:", error, "in", file, "@", line);
    }
}
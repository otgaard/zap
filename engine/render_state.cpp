/* Created by Darren Otgaar on 2016/10/12. http://www.github.com/otgaard/zap */
#include "render_state.hpp"
#include "gl_api.hpp"

using namespace zap::engine;
using namespace zap::engine::gl;

GLenum gl_src_blend_mode[(int)render_state::blend_state::src_blend_mode::SBM_SIZE] = {
        GL_ZERO,
        GL_ONE,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_SRC_ALPHA_SATURATE,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA
};

GLenum gl_dst_blend_mode[(int)render_state::blend_state::dst_blend_mode::DBM_SIZE] = {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_ALPHA,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA
};

GLenum gl_compare_mode[(int)render_state::blend_state::compare_mode::CM_SIZE] = {
        GL_NEVER,
        GL_LESS,
        GL_EQUAL,
        GL_LEQUAL,
        GL_GREATER,
        GL_NOTEQUAL,
        GL_GEQUAL,
        GL_ALWAYS
};

void render_state::blend_state::set() {
    if(blend_enabled) {
        glEnable(GL_BLEND);
        glBlendFunc(gl_src_blend_mode[(int) src_mode], gl_dst_blend_mode[(int) dst_mode]);
        glBlendColor(colour.x, colour.y, colour.z, colour.w);
        gl_error_check();
    }

    if(compare_enabled) {
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(gl_compare_mode[(int)cmp_mode], reference);
        gl_error_check();
    }
}

void render_state::blend_state::clear() {
    if(blend_enabled) glDisable(GL_BLEND);
    if(compare_enabled) glDisable(GL_ALPHA_TEST);
}

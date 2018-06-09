/* Created by Darren Otgaar on 2017/08/08. http://www.github.com/otgaard/zap */
#include "state_stack.hpp"
#include "gl_api.hpp"

using namespace zap;
using namespace zap::engine;
using namespace zap::engine::gl;

extern const GLenum gl_src_blend_mode[(int)render_state::blend_state::src_blend_mode::SBM_SIZE];
extern const GLenum gl_dst_blend_mode[(int)render_state::blend_state::dst_blend_mode::DBM_SIZE];
extern const GLenum gl_polygon_mode[(int)render_state::rasterisation_state::polygon_mode::PM_SIZE];
extern const GLenum gl_compare_mode[(int)render_state::compare_mode::CM_SIZE];
extern const GLenum gl_cull_mode[(int)render_state::rasterisation_state::cull_mode::CM_SIZE];
extern const GLenum gl_operation_type[(int)render_state::stencil_state::operation_type::OT_SIZE];

state_stack::state_stack() : base_state_(RS_BLEND | RS_DEPTH | RS_SCISSOR | RS_RASTERISATION | RS_STENCIL),
                             clear_colour_{0.f, 0.f, 0.f, 0.f} {
}

bool state_stack::initialise() {
    initialise(base_state_.blend());
    blend_stack_.push(base_state_.blend());

    initialise(base_state_.depth());
    depth_stack_.push(base_state_.depth());

    initialise(base_state_.scissor());
    scissor_stack_.push(base_state_.scissor());

    initialise(base_state_.rasterisation());
    rasterisation_stack_.push(base_state_.rasterisation());

    initialise(base_state_.stencil());
    stencil_stack_.push(base_state_.stencil());

    stack_.push(&base_state_);

    clear_colour(clear_colour_);

    return true;
}

void zap::engine::state_stack::clear_colour(float r, float g, float b, float a) {
    clear_colour_.set(r, g, b, a);
    glClearColor(clear_colour_.x, clear_colour_.y, clear_colour_.z, clear_colour_.w);
}

void state_stack::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    clear();
    glClearColor(clear_colour_.x, clear_colour_.y, clear_colour_.z, clear_colour_.w);
}

void zap::engine::state_stack::clear(bool colour, bool depth, bool stencil) {
    GLenum flags = 0;
    if(colour) flags |= GL_COLOR_BUFFER_BIT;
    if(depth) flags |= GL_DEPTH_BUFFER_BIT;
    if(stencil) flags |= GL_STENCIL_BUFFER_BIT;
    glClear(flags);
}

void zap::engine::state_stack::push_state(const render_state* state) {
    assert(state != nullptr && "Cannot push a null state!");
    if(state != peek()) {
        if(state->blend() != nullptr) push_state(state->blend());
        if(state->depth() != nullptr) push_state(state->depth());
        if(state->scissor() != nullptr) push_state(state->scissor());
        if(state->rasterisation() != nullptr) push_state(state->rasterisation());
        if(state->stencil() != nullptr) push_state(state->stencil());

        stack_.push(state);
    }
}

void zap::engine::state_stack::pop() {
    if(stack_.size() > 1) {
        if(peek_blend_state() != nullptr) pop_blend_state();
        if(peek_depth_state() != nullptr) pop_depth_state();
        if(peek_scissor_state() != nullptr) pop_scissor_state();
        if(peek_rasterisation_state() != nullptr) pop_rasterisation_state();
        if(peek_stencil_state() != nullptr) pop_stencil_state();

        stack_.pop();
    }
}

void zap::engine::state_stack::push_state(const blend_state* state) {
    if(state == nullptr || state == curr_blend_state()) return;
    transition(curr_blend_state(), state);
    blend_stack_.push(state);
}

void zap::engine::state_stack::pop_blend_state() {
    if(blend_stack_.size() > 1) {
        auto curr = curr_blend_state();
        blend_stack_.pop();
        auto prev = curr_blend_state();
        transition(curr, prev);
    }
}

void zap::engine::state_stack::initialise(const blend_state* state) {
    if(state == nullptr) return;
    if(state->enabled) glEnable(GL_BLEND);
    else               glDisable(GL_BLEND);
    glBlendFunc(gl_src_blend_mode[(int)state->src_mode], gl_dst_blend_mode[(int)state->dst_mode]);
    glBlendColor(state->colour.x, state->colour.y, state->colour.z, state->colour.w);

    gl_error_check();
}

void zap::engine::state_stack::transition(const blend_state* source, const blend_state* target) {
    if(target->enabled) {
        if(!source->enabled) glEnable(GL_BLEND);
    } else {
        if(source->enabled)  glDisable(GL_BLEND);
    }

    if(target->src_mode != source->src_mode || target->dst_mode != source->dst_mode)
        glBlendFunc(gl_src_blend_mode[(int)target->src_mode], gl_dst_blend_mode[(int)target->dst_mode]);
    if(target->colour != source->colour)
        glBlendColor(target->colour.x, target->colour.y, target->colour.z, target->colour.w);

    gl_error_check();
}

void state_stack::push_state(const state_stack::depth_state* state) {
    if(state == nullptr || state == curr_depth_state()) return;
    transition(curr_depth_state(), state);
    depth_stack_.push(state);
}

void state_stack::pop_depth_state() {
    if(depth_stack_.size() > 1) {
        auto curr = curr_depth_state();
        depth_stack_.pop();
        auto prev = curr_depth_state();
        transition(curr, prev);
    }
}

void state_stack::initialise(const state_stack::depth_state* state) {
    if(state == nullptr) return;
    if(state->enabled) glEnable(GL_DEPTH_TEST);
    else               glDisable(GL_DEPTH_TEST);
    glDepthMask(state->writable ? GL_TRUE : GL_FALSE);
    glDepthFunc(gl_compare_mode[(int)state->cmp_mode]);
    assert(0 <= state->clear_depth && state->clear_depth <= 1.f && "clear_depth must be clamped to [0, 1]");
    glClearDepth(state->clear_depth);

    gl_error_check();
}

void state_stack::transition(const state_stack::depth_state* source, const state_stack::depth_state* target) {
    if(target->enabled) {
        if(!source->enabled) glEnable(GL_DEPTH_TEST);
    } else {
        if(source->enabled)  glDisable(GL_DEPTH_TEST);
    }

    if(target->writable != source->writable) glDepthMask(target->writable ? GL_TRUE : GL_FALSE);
    if(target->cmp_mode != source->cmp_mode) glDepthFunc(gl_compare_mode[(int)target->cmp_mode]);
    if(target->clear_depth != source->clear_depth) glClearDepth(target->clear_depth);

    gl_error_check();
}

void state_stack::push_state(const scissor_state* state) {
    if(state == nullptr || state == curr_scissor_state()) return;
    transition(curr_scissor_state(), state);
    scissor_stack_.push(state);
}

void state_stack::pop_scissor_state() {
    if(scissor_stack_.size() > 1) {
        auto curr = curr_scissor_state();
        scissor_stack_.pop();
        auto prev = curr_scissor_state();
        transition(curr, prev);
    }
}

void state_stack::initialise(const scissor_state* state) {
    if(state == nullptr) return;
    if(state->enabled) {
        glEnable(GL_SCISSOR_TEST);
        glScissor(state->x, state->y, state->width, state->height);
    } else {
        glDisable(GL_SCISSOR_TEST);
    }
    // Default scissor state is entire viewport
}

void state_stack::transition(const scissor_state* source, const scissor_state* target) {
    if(target->enabled) {
        if(!source->enabled) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(target->x, target->y, target->width, target->height);
        }
    } else {
        if(source->enabled) glDisable(GL_SCISSOR_TEST);
    }
}

void state_stack::push_state(const rasterisation_state* state) {
    if(state == nullptr || state == curr_rasterisation_state()) return;
    transition(curr_rasterisation_state(), state);
    rasterisation_stack_.push(state);
}

void state_stack::pop_rasterisation_state() {
    if(rasterisation_stack_.size() > 1) {
        auto curr = curr_rasterisation_state();
        rasterisation_stack_.pop();
        auto prev = curr_rasterisation_state();
        transition(curr, prev);
    }
}

void state_stack::initialise(const rasterisation_state* state) {
    if(state == nullptr) return;
    glLineWidth(state->line_width);
    glPointSize(state->point_size);
    glPolygonMode(GL_FRONT_AND_BACK, gl_polygon_mode[(int)state->poly_mode]);
    glCullFace(gl_cull_mode[(int)state->cull_face]);
    glPrimitiveRestartIndex(state->primitive_restart);
    glColorMask(state->colour_mask.x, state->colour_mask.y, state->colour_mask.z, state->colour_mask.w);
    if(state->enable_program_point_size) glEnable(GL_PROGRAM_POINT_SIZE);
    if(state->enable_vertex_point_size) glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    if(state->enable_point_smooth) glEnable(GL_POINT_SMOOTH);
    if(state->enable_line_smooth) glEnable(GL_LINE_SMOOTH);
    if(state->enable_polygon_smooth) glEnable(GL_POLYGON_SMOOTH);
    if(state->enable_primitive_restart) glEnable(GL_PRIMITIVE_RESTART);
    if(state->enable_multisampling) glEnable(GL_MULTISAMPLE);
    if(state->enable_culling) glEnable(GL_CULL_FACE);

    gl_error_check();
}

void state_stack::transition(const rasterisation_state* source, const rasterisation_state* target) {
    if(target->line_width != source->line_width) glLineWidth(target->line_width);
    if(target->point_size != source->point_size) glPointSize(target->point_size);

    if(target->poly_mode != source->poly_mode) glPolygonMode(GL_FRONT_AND_BACK, gl_polygon_mode[(int)target->poly_mode]);
    if(target->cull_face != source->cull_face) glCullFace(gl_cull_mode[(int)target->cull_face]);
    if(target->primitive_restart != source->primitive_restart) glPrimitiveRestartIndex(target->primitive_restart);
    if(target->colour_mask != source->colour_mask) glColorMask(target->colour_mask.x, target->colour_mask.y, target->colour_mask.z, target->colour_mask.w);

    if(target->enable_program_point_size != source->enable_program_point_size) {
        if(!source->enable_program_point_size) glEnable(GL_PROGRAM_POINT_SIZE);
        else glDisable(GL_PROGRAM_POINT_SIZE);
    }

    if(target->enable_vertex_point_size != source->enable_vertex_point_size) {
        if(!source->enable_vertex_point_size) glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        else glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
    }

    if(target->enable_point_smooth != source->enable_point_smooth) {
        if(!source->enable_point_smooth) glEnable(GL_PROGRAM_POINT_SIZE);
        else glDisable(GL_PROGRAM_POINT_SIZE);
    }

    if(target->enable_line_smooth != source->enable_line_smooth) {
        if(!source->enable_line_smooth) glEnable(GL_LINE_SMOOTH);
        else glDisable(GL_LINE_SMOOTH);
    }

    if(target->enable_polygon_smooth != source->enable_polygon_smooth) {
        if(!source->enable_polygon_smooth) glEnable(GL_POLYGON_SMOOTH);
        else glDisable(GL_POLYGON_SMOOTH);
    }

    if(target->enable_primitive_restart != source->enable_primitive_restart) {
        if(!source->enable_primitive_restart) glEnable(GL_PRIMITIVE_RESTART);
        else glDisable(GL_PRIMITIVE_RESTART);
    }

    if(target->enable_multisampling != source->enable_multisampling) {
        if(!source->enable_multisampling) glEnable(GL_MULTISAMPLE);
        else glDisable(GL_MULTISAMPLE);
    }

    if(target->enable_culling != source->enable_culling) {
        if(!source->enable_culling) glEnable(GL_CULL_FACE);
        else glDisable(GL_CULL_FACE);
    }

    gl_error_check();
}

void state_stack::push_state(const stencil_state* state) {
    if(state == nullptr || state == curr_stencil_state()) return;
    transition(curr_stencil_state(), state);
    stencil_stack_.push(state);
}

void state_stack::pop_stencil_state() {
    if(stencil_stack_.size() > 1) {
        auto curr = curr_stencil_state();
        stencil_stack_.pop();
        auto prev = curr_stencil_state();
        transition(curr, prev);
    }
}

void state_stack::initialise(const stencil_state* state) {
    if(state == nullptr) return;

    if(state->enabled) glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);
    glStencilFunc(gl_compare_mode[int(state->cmp_mode)], state->reference, state->fnc_mask);
    glStencilMask(state->write_mask);
    glStencilOp(
            gl_operation_type[int(state->on_fail)],
            gl_operation_type[int(state->on_zfail)],
            gl_operation_type[int(state->on_zpass)]);
    glClearStencil(state->clear);
    gl_error_check();
}

void state_stack::transition(const stencil_state* source, const stencil_state* target) {
    if(target->enabled != source->enabled) {
        if(!source->enabled) glEnable(GL_STENCIL_TEST);
        else glDisable(GL_STENCIL_TEST);
    }

    gl_error_check();

    if(target->cmp_mode != source->cmp_mode ||
       target->reference != source->reference ||
       target->fnc_mask != source->fnc_mask) {
        glStencilFunc(gl_compare_mode[int(target->cmp_mode)], target->reference, target->fnc_mask);
    }

    gl_error_check();

    if(target->write_mask != source->write_mask) glStencilMask(target->write_mask);

    gl_error_check();

    if(target->on_fail != source->on_fail ||
       target->on_zfail != source->on_zfail ||
       target->on_zpass != source->on_zpass) {
        glStencilOp(
                gl_operation_type[int(target->on_fail)],
                gl_operation_type[int(target->on_zfail)],
                gl_operation_type[int(target->on_zpass)]);
    }

    gl_error_check();

    if(target->clear != source->clear) {
        glClearStencil(target->clear);
    }

    gl_error_check();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPENGL TABLES

const GLenum gl_src_blend_mode[(int)render_state::blend_state::src_blend_mode::SBM_SIZE] = {
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

const GLenum gl_dst_blend_mode[(int)render_state::blend_state::dst_blend_mode::DBM_SIZE] = {
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

const GLenum gl_compare_mode[(int)render_state::compare_mode::CM_SIZE] = {
        GL_NEVER,
        GL_LESS,
        GL_EQUAL,
        GL_LEQUAL,
        GL_GREATER,
        GL_NOTEQUAL,
        GL_GEQUAL,
        GL_ALWAYS
};

const GLenum gl_polygon_mode[(int)render_state::rasterisation_state::polygon_mode::PM_SIZE] = {
        GL_POINT,
        GL_LINE,
        GL_FILL
};

const GLenum gl_cull_mode[(int)render_state::rasterisation_state::cull_mode::CM_SIZE] = {
        GL_FRONT,
        GL_BACK,
        GL_FRONT_AND_BACK
};

const GLenum gl_operation_type[(int)render_state::stencil_state::operation_type::OT_SIZE] = {
        GL_KEEP,
        GL_ZERO,
        GL_REPLACE,
        GL_INCR,
        GL_DECR,
        GL_INVERT
};


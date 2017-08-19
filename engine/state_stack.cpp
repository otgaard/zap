/* Created by Darren Otgaar on 2017/08/08. http://www.github.com/otgaard/zap */
#include "state_stack.hpp"
#include "gl_api.hpp"

using namespace zap;
using namespace zap::engine;
using namespace zap::engine::gl;

extern const GLenum gl_src_blend_mode[(int)render_state::blend_state::src_blend_mode::SBM_SIZE];
extern const GLenum gl_dst_blend_mode[(int)render_state::blend_state::dst_blend_mode::DBM_SIZE];
extern const GLenum gl_compare_mode[(int)render_state::compare_mode::CM_SIZE];

state_stack::state_stack() : base_state_(true, true) {
}

bool state_stack::initialise() {
    initialise(base_state_.get_blend_state());
    blend_stack_.push(base_state_.get_blend_state());

    initialise(base_state_.get_depth_state());
    depth_stack_.push(base_state_.get_depth_state());

    stack_.push(&base_state_);
    return true;
}

void zap::engine::state_stack::push_state(const render_state* state) {
    assert(state != nullptr && "Cannot push a null state!");
    if(state != peek()) {
        if(state->get_blend_state() != nullptr) push_state(state->get_blend_state());
        if(state->get_depth_state() != nullptr) push_state(state->get_depth_state());

        stack_.push(state);
    }
}

void zap::engine::state_stack::pop() {
    if(stack_.size() > 1) {
        if(peek_blend_state() != nullptr) pop_blend_state();
        if(peek_depth_state() != nullptr) pop_depth_state();

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

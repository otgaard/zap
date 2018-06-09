/* Created by Darren Otgaar on 2017/08/08. http://www.github.com/otgaard/zap */
#ifndef ZAP_STATE_STACK_HPP
#define ZAP_STATE_STACK_HPP

// The state_stack stores render_state objects and represents the current state of the OpenGL blend, alpha, depth,
// culling, multisampling, and wireframe render states.  The state transitions set the minimum required to transition
// from one state to the next.

#include <stack>
#include "render_state.hpp"

namespace zap { namespace engine {

class ZAPENGINE_EXPORT state_stack {
public:
    constexpr static size_t max_depth = 16;
    using vec4f = maths::vec4f;
    using stack_t = std::stack<const render_state*>;
    using blend_state = render_state::blend_state;
    using depth_state = render_state::depth_state;
    using scissor_state = render_state::scissor_state;
    using rasterisation_state = render_state::rasterisation_state;
    using stencil_state = render_state::stencil_state;

    state_stack();

    bool initialise();
    bool is_initialised() const { return !blend_stack_.empty(); }

    void push_state(const render_state* state);
    const render_state* peek() const { return stack_.top(); }
    void pop();

    const blend_state* peek_blend_state() const { return peek()->blend(); }
    const blend_state* curr_blend_state() const { return blend_stack_.top(); }

    const depth_state* peek_depth_state() const { return peek()->depth(); }
    const depth_state* curr_depth_state() const { return depth_stack_.top(); }

    const scissor_state* peek_scissor_state() const { return peek()->scissor(); }
    const scissor_state* curr_scissor_state() const { return scissor_stack_.top(); }

    const rasterisation_state* peek_rasterisation_state() const { return peek()->rasterisation(); }
    const rasterisation_state* curr_rasterisation_state() const { return rasterisation_stack_.top(); }

    const stencil_state* peek_stencil_state() const { return peek()->stencil(); }
    const stencil_state* curr_stencil_state() const { return stencil_stack_.top(); }

    void clear_colour(float r, float g, float b, float a);
    void clear_colour(const vec4f& v) { clear_colour(v.x, v.y, v.z, v.w); }
    vec4f clear_colour() const { return clear_colour_; }
    void clear(float r, float g, float b, float a);
    void clear(const vec4f& v) { clear(v.x, v.y, v.z, v.w); }
    void clear(bool colour=true, bool depth=true, bool stencil=true);

private:
    // Blending state
    void push_state(const blend_state* state);
    void pop_blend_state();
    void initialise(const blend_state* state);
    void transition(const blend_state* source, const blend_state* target);

    // Depth state
    void push_state(const depth_state* state);
    void pop_depth_state();
    void initialise(const depth_state* state);
    void transition(const depth_state* source, const depth_state* target);

    // Scissor state
    void push_state(const scissor_state* state);
    void pop_scissor_state();
    void initialise(const scissor_state* state);
    void transition(const scissor_state* source, const scissor_state* target);

    // Rasterisation state
    void push_state(const rasterisation_state* state);
    void pop_rasterisation_state();
    void initialise(const rasterisation_state* state);
    void transition(const rasterisation_state* source, const rasterisation_state* target);

    // Stencil state
    void push_state(const stencil_state* state);
    void pop_stencil_state();
    void initialise(const stencil_state* state);
    void transition(const stencil_state* source, const stencil_state* target);

    stack_t stack_;
    render_state base_state_;
    std::stack<const blend_state*> blend_stack_;
    std::stack<const depth_state*> depth_stack_;
    std::stack<const scissor_state*> scissor_stack_;
    std::stack<const rasterisation_state*> rasterisation_stack_;
    std::stack<const stencil_state*> stencil_stack_;
    vec4f clear_colour_;
};

}}

#endif //ZAP_STATE_STACK_HPP

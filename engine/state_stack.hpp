/* Created by Darren Otgaar on 2017/08/08. http://www.github.com/otgaard/zap */
#ifndef ZAP_STATE_STACK_HPP
#define ZAP_STATE_STACK_HPP

// The state_stack stores render_state objects and represents the current state of the OpenGL blend, alpha, depth,
// culling, multisampling, and wireframe render states.  The state transitions set the minimum required to transition
// from one state to the next.

#include <stack>
#include "render_state.hpp"

namespace zap { namespace engine {

class state_stack {
public:
    constexpr static size_t max_depth = 16;
    using stack_t = std::stack<const render_state*>;
    using blend_state = render_state::blend_state;

    void push_state(const render_state* state);
    const render_state* peek() const { return stack_.top(); }
    void pop();

    const blend_state* peek_blend_state() const { return peek()->get_blend_state(); }

protected:
    void push_blend_state(const blend_state* state);
    void pop_blend_state();
    void initialise(const blend_state* state);
    void transition(const blend_state* source, const blend_state* target);

    stack_t stack_;
};

}}

#endif //ZAP_STATE_STACK_HPP

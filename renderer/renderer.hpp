/* Created by Darren Otgaar on 2016/08/14. http://www.github.com/otgaard/zap */
#ifndef ZAP_RENDERER_HPP
#define ZAP_RENDERER_HPP

// The OpenGL renderer

#include <engine/state_stack.hpp>

namespace zap { namespace renderer {
    class renderer {
    public:
        using state_stack = engine::state_stack;
        using render_state = engine::render_state;

        renderer() = default;
        ~renderer() = default;

        static renderer* instance();

        bool initialise();

        void push_state(const render_state* rndr_state) { state_stack_.push_state(rndr_state); }
        const render_state* curr_state() const { return state_stack_.peek(); }
        void pop_state() { state_stack_.pop(); }

    protected:
        static renderer* inst_;

        bool initialised_ = false;
        state_stack state_stack_;
    };
}}

#endif //ZAP_RENDERER_HPP

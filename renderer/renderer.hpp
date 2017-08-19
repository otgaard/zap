/* Created by Darren Otgaar on 2016/08/14. http://www.github.com/otgaard/zap */
#ifndef ZAP_RENDERER_HPP
#define ZAP_RENDERER_HPP

// The OpenGL renderer

#include <engine/state_stack.hpp>
#include <scene_graph/visual.hpp>
#include <renderer/render_context.hpp>

namespace zap { namespace renderer {
    class renderer {
    public:
        using state_stack = engine::state_stack;
        using render_state = engine::render_state;

        renderer() = default;
        ~renderer() = default;

        bool initialise();

        template <typename SpatialT>
        void draw(const scene_graph::visual<SpatialT>& v, const render_context* context_ptr, const render_args& args) {
            context_ptr->bind(args);
            if(context_ptr->get_state() != nullptr) push_state(context_ptr->get_state());
            v.get_mesh()->bind();
            v.get_mesh()->draw();
            v.get_mesh()->release();
            if(context_ptr->get_state() != nullptr) pop_state();
            context_ptr->release();
        }

        void push_state(const render_state* rndr_state) { state_stack_.push_state(rndr_state); }
        const render_state* curr_state() const { return state_stack_.peek(); }
        void pop_state() { state_stack_.pop(); }

    protected:
        bool initialised_ = false;
        state_stack state_stack_;
    };
}}

#endif //ZAP_RENDERER_HPP

/* Created by Darren Otgaar on 2016/08/14. http://www.github.com/otgaard/zap */
#ifndef ZAP_RENDERER_HPP
#define ZAP_RENDERER_HPP

// The OpenGL renderer

#include <renderer/rndr.hpp>
#include <engine/state_stack.hpp>
#include <renderer/scene_graph/visual.hpp>
#include <renderer/render_context.hpp>

namespace zap { namespace renderer {
    class ZAPRENDERER_EXPORT renderer {
    public:
        using mesh_base = engine::mesh_base;
        using state_stack = engine::state_stack;
        using render_state = engine::render_state;
        using primitive_type = engine::primitive_type;

        renderer() = default;
        ~renderer() = default;

        bool initialise();

        void resize(int width, int height) { screen_width_ = width; screen_height_ = height; }

        void draw(primitive_type type, const mesh_base* mesh_ptr, const render_context* context_ptr);
        void draw(primitive_type type, const mesh_base* mesh_ptr, const render_context* context_ptr, uint32_t first, uint32_t count);
        void draw(primitive_type type, const mesh_base* mesh_ptr, const render_context* context_ptr, uint32_t first, uint32_t count, uint32_t instances);
        void draw(primitive_type type, const mesh_base* mesh_ptr, const render_context* context_ptr, uint32_t first, uint32_t count, uint32_t instances, uint32_t offset);
        void draw(primitive_type type, const mesh_base* mesh_ptr, const render_context* context_ptr, const render_args& args);

        template <typename SpatialT>
        void draw(const scene_graph::visual<SpatialT>& v, const render_context* context_ptr, const render_args& args) {
            draw(v.get_type(), v.get_mesh(), context_ptr, args);
        }

        template <typename SpatialT>
        void draw(const scene_graph::visual<SpatialT>& v, const render_args& args) {
            draw(v.get_type(), v.get_mesh(), args.get_context(), args);
        }

        void push_state(const render_state* rndr_state) { state_stack_.push_state(rndr_state); }
        const render_state* curr_state() const { return state_stack_.peek(); }
        void pop_state() { state_stack_.pop(); }
        // TODO: Remove this interface
        state_stack* get_state_stack() { return &state_stack_; }

        const render_context* curr_context() const { return curr_context_; }
        void release_curr_context() {
            if(curr_context_ != nullptr) {
                if(curr_context_->get_state() != nullptr) pop_state();
                curr_context_->release();
                curr_context_ = nullptr;
            }
        }
        const mesh_base* curr_mesh() const { return curr_mesh_; }
        void release_curr_mesh() {
            if(curr_mesh_ != nullptr) {
                curr_mesh_->release();
                curr_mesh_ = nullptr;
            }
        }

    private:
        void transition(const mesh_base* mesh_ptr, const render_context* context_ptr, const render_args* args_ptr=nullptr);

        bool initialised_ = false;
        int screen_width_ = 0, screen_height_ = 0;
        const render_context* curr_context_ = nullptr;
        const mesh_base* curr_mesh_ = nullptr;
        state_stack state_stack_;
    };
}}

#endif //ZAP_RENDERER_HPP

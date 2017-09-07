/* Created by Darren Otgaar on 2016/08/14. http://www.github.com/otgaard/zap */
#include "renderer.hpp"

using namespace zap::engine;
using namespace zap::renderer;

bool renderer::initialise() {
    initialised_ = state_stack_.initialise();
    return initialised_;
}

void renderer::transition(const renderer::mesh_base* mesh_ptr, const render_context* context_ptr, const render_args* args_ptr) {
    if(curr_context_ != context_ptr) {  // Check if the context is already bound and re-use, otherwise detach first
        if(curr_context_ != nullptr) {
            if(curr_context_->get_state() != nullptr) pop_state();
            curr_context_->release();
        }
        curr_context_ = context_ptr;
        if(args_ptr != nullptr) curr_context_->bind(*args_ptr);
        else                    curr_context_->bind();
        if(curr_context_->get_state() != nullptr) push_state(curr_context_->get_state());
    } else {
        if(args_ptr != nullptr) curr_context_->bind(*args_ptr);
        else                    curr_context_->rebind();
    }

    if(curr_mesh_ != mesh_ptr) {
        if(curr_mesh_ != nullptr) curr_mesh_->release();
        curr_mesh_ = mesh_ptr;
        curr_mesh_->bind();
    }

    gl_error_check();
}


void renderer::draw(const zap::engine::mesh_base* mesh_ptr, const zap::renderer::render_context* context_ptr,
                              const zap::renderer::render_args& args) {
    transition(mesh_ptr, context_ptr, &args);
    mesh_ptr->draw();
}

void renderer::draw(const renderer::mesh_base* mesh_ptr, const render_context* context_ptr) {
    transition(mesh_ptr, context_ptr);
    mesh_ptr->draw();
}

void renderer::draw(const renderer::mesh_base* mesh_ptr, const render_context* context_ptr, uint32_t first, uint32_t count) {
    transition(mesh_ptr, context_ptr);
    if(!mesh_ptr->is_indexed()) mesh_ptr->draw_arrays_impl(mesh_ptr->get_primitive_type(), first, count);
    else mesh_ptr->draw_elements_impl(mesh_ptr->get_primitive_type(), mesh_ptr->get_index_type(), first, count);
}

void renderer::draw(const renderer::mesh_base* mesh_ptr, const render_context* context_ptr, uint32_t first, uint32_t count, uint32_t instances) {
    transition(mesh_ptr, context_ptr);
    if(!mesh_ptr->is_indexed()) mesh_ptr->draw_arrays_inst_impl(mesh_ptr->get_primitive_type(), first, count, instances);
    else mesh_ptr->draw_elements_inst_impl(mesh_ptr->get_primitive_type(), mesh_ptr->get_index_type(), first, count, instances);
}

void
renderer::draw(const renderer::mesh_base* mesh_ptr, const render_context* context_ptr, uint32_t first, uint32_t count,
               uint32_t instances, uint32_t offset) {
    transition(mesh_ptr, context_ptr);
    if(!mesh_ptr->is_indexed()) mesh_ptr->draw_arrays_inst_baseinst_impl(mesh_ptr->get_primitive_type(), first, count, instances, offset);
    else mesh_ptr->draw_elements_inst_baseinst_impl(mesh_ptr->get_primitive_type(), mesh_ptr->get_index_type(), first, count, instances, offset);
}

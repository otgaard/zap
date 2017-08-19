//
// Created by Darren Otgaar on 2017/07/30.
//

#include "render_context.hpp"
#include "engine/gl_api.hpp"
#include "render_args.hpp"
#include "tools/log.hpp"
#include "renderer.hpp"

using namespace zap;
using namespace zap::engine;
using namespace zap::renderer;

void render_context::bind(zap::renderer::renderer& rndr) const {
    program_->bind();
    is_bound_ = true;
    if(!textures_.empty()) {
        if(samplers_.empty()) {
            for(size_t i = 0; i != textures_.size(); ++i) textures_[i]->bind(i);
        } else {
            // TODO: Bind only the required textures
            for(size_t i = 0; i != textures_.size(); ++i) {
                textures_[i]->bind(i);
                samplers_[i]->bind(i);
            }
        }
    }

    if(!ubuffers_.empty()) {
        for(size_t i = 0; i != ubuffers_.size(); ++i) {
            // TODO: Try to avoid these two steps per frame by reusing same location across shader programs
            const auto& name = ubname_[i];
            auto it = std::find_if(blocks_.begin(), blocks_.end(), [&name](const auto& b) { return name == b.name; });
            if(it == blocks_.end()) continue;
            auto idx = it - blocks_.begin();
            program_->bind_block(blocks_[idx].index, i);
            ubuffers_[i]->bind_point(i);
            ubuffers_[i]->bind();
        }
    }

    if(dirty_) {
        for(size_t i = 0; i != parameters_.size(); ++i) {
            if(dirty_flags_[i]) {
                program_->bind_uniform(parameters_[i].location, parameters_[i].type, parameters_[i].count, &uniforms_[offsets_[i]]);
                if(gl_error_check()) LOG_ERR("Error binding parameter:", parameters_[i].name, gl::gl_typename(parameters_[i].type), parameters_[i].count);
                dirty_flags_[i] = false;
            }
        }
        dirty_ = false;
    }

    if(rndr_state_) rndr.push_state(rndr_state_);
}

void render_context::release(zap::renderer::renderer& rndr) const {
    if(rndr_state_) rndr.pop_state();

    if(!textures_.empty()) {
        if(samplers_.empty()) {
            for(size_t i = 0; i != textures_.size(); ++i) {
                textures_[i]->release();
            }
        } else {
            for(size_t i = 0; i != textures_.size(); ++i) {
                samplers_[i]->release(i);
                textures_[i]->release();
            }
        }
    }

    if(!ubuffers_.empty()) {
        for(size_t i = 0; i != ubuffers_.size(); ++i) ubuffers_[i]->release();
    }

    program_->release();
    is_bound_ = false;
}

void render_context::bind_arguments(const render_args& args) const {

}

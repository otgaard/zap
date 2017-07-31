//
// Created by Darren Otgaar on 2017/07/30.
//

#include "render_context.hpp"
#include "engine/gl_api.hpp"
#include "tools/log.hpp"

using namespace zap;
using namespace zap::engine;
using namespace zap::renderer;

void render_context::bind() const {
    program_->bind();
    if(!textures_.empty()) {
        if(samplers_.empty()) {
            for(int i = 0; i != textures_.size(); ++i) textures_[i]->bind(i);
        } else {
            for(int i = 0; i != textures_.size(); ++i) {
                textures_[i]->bind(i);
                samplers_[i]->bind(i);
            }
        }
    }

    if(dirty_) {
        for(int i = 0; i != parameters_.size(); ++i) {
            if(dirty_flags_[i]) {
                program_->bind_uniform(parameters_[i].location, parameters_[i].type, parameters_[i].count, &uniforms_[offsets_[i]]);
                if(gl_error_check()) LOG_ERR("Error binding parameter:", parameters_[i].name, gl::gl_typename(parameters_[i].type), parameters_[i].count);
                dirty_flags_[i] = false;
            }
        }
        dirty_ = false;
    }
}

void render_context::release() const {
    if(!textures_.empty()) {
        if(samplers_.empty()) {
            for(int i = 0; i != textures_.size(); ++i) {
                textures_[i]->release();
            }
        } else {
            for(int i = 0; i != textures_.size(); ++i) {
                samplers_[i]->release(i);
                textures_[i]->release();
            }
        }
    }
    program_->release();
}

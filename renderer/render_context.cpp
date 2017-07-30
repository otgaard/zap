//
// Created by Darren Otgaar on 2017/07/30.
//

#include "render_context.hpp"

using namespace zap;
using namespace zap::renderer;

void render_context::bind() const {
    program_->bind();
    if(!textures_.empty()) {
        for(int i = 0; i != textures_.size(); ++i) {
            textures_[i]->bind(i);
        }
    }
}

void render_context::release() const {
    if(!textures_.empty()) {
        for(int i = 0; i != textures_.size(); ++i) {
            textures_[i]->release();
        }
    }
    program_->release();
}

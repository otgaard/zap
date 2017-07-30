//
// Created by Darren Otgaar on 2017/07/30.
//

#include "render_context.hpp"

using namespace zap;
using namespace zap::renderer;

void render_context::bind() const {
    program_->bind();
}

void render_context::release() const {
    program_->release();
}

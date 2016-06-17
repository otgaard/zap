/* Created by Darren Otgaar on 2016/05/27. http://www.github.com/otgaard/zap */
#include "framebuffer.hpp"
#include "gl_api.hpp"

using namespace zap::engine;
using namespace zap::engine::gl;

bool framebuffer::allocate() {
    assert(target_count_ > 0 || depthstencil_ && "At least one target required for framebuffer");
    glGenFramebuffers(1, &framebuffer_);
    LOG("Framebuffer Allocated: ", framebuffer_);
    return is_allocated() && !gl_error_check();
}

void framebuffer::deallocate() {

}

void framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
}

void framebuffer::release() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool framebuffer::is_bound() const {
    return false;
}

bool framebuffer::initialise() {
    if(attachments_.size() > 0) return false;

    bind();

    attachments_.reserve(target_count_);
    for(size_t i = 0; i != target_count_; ++i) {
        attachments_.emplace_back(texture());
        attachments_[i].allocate();
        attachments_[i].initialise(width_, height_, pix_format_, pix_dtype_, mipmaps_);
    }

    return false;
}

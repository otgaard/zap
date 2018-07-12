//
// Created by Darren Otgaar on 2018/07/12.
//

#include "renderbuffer.hpp"
#include "gl_api.hpp"

using namespace zap::engine;
using namespace zap::engine::gl;

bool renderbuffer::allocate() {
    glGenRenderbuffers(1, &id_);
    LOG("Renderbuffer Allocated:", id_);
    gl_error_check();
    return is_allocated();
}

bool renderbuffer::deallocate() {
    if(!is_allocated()) return true;
    glDeleteRenderbuffers(1, &id_);
    LOG("Renderbuffer Deallocated:", id_);
    gl_error_check();
    id_ = INVALID_RESOURCE;
    return true;
}

bool renderbuffer::initialise() {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, int_fmt_, width(), height());
    if(gl_error_check()) {
        LOG_ERR("Failed to initialise renderbuffer", id_);
        return false;
    }
    release();
    return true;
}

bool renderbuffer::initialise(pixel_format pf, pixel_datatype dt, int32_t width, int32_t height) {
    int_fmt_ = gl_internal_format(pf, dt);
    w_ = width;
    h_ = height;
    d_ = 1;
    return initialise();
}

void renderbuffer::bind() {
    glBindRenderbuffer(GL_RENDERBUFFER, id_);
    gl_error_check();
}

void renderbuffer::release() {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

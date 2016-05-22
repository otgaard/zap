/* Created by Darren Otgaar on 2016/05/07. http://www.github.com/otgaard/zap */
#include "texture.hpp"
#include "gl_api.hpp"

using namespace zap::engine;
using namespace zap::engine::gl;

texture::~texture() {
    if(is_allocated()) deallocate();
}

bool texture::allocate() {
    glGenTextures(1, &id_);
    LOG("Texture Allocated:", id_);
    gl_error_check();
    return is_allocated();
}

bool texture::deallocate() {
    glDeleteTextures(1, &id_);
    LOG("Texture Deallocated:", id_);
    gl_error_check();
    id_ = INVALID_RESOURCE;
    return true;
}

void texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, id_);      // TODO: Oi!
    gl_error_check();
}

void texture::release() const {
    glBindTexture(GL_TEXTURE_2D, 0);        // TODO: Oi!
}

bool texture::is_bound() const {
    LOG_WARN("OpenGL query operation invoked");
    GLint bound = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);   // TODO: Generic for tex type
    return id_ == (resource_t)bound;
}

bool texture::initialise(size_t width, size_t height, const std::vector<rgb332_t>& buffer, bool generate_mipmaps) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE_3_3_2, buffer.data());
    if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);
    release();
    return !gl_error_check();
}


bool texture::initialise(size_t width, size_t height, const std::vector<rgb888_t>& buffer, bool generate_mipmaps) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);
    release();
    return !gl_error_check();
}
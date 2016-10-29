/* Created by Darren Otgaar on 2016/05/07. http://www.github.com/otgaard/zap */
#include "texture.hpp"
#include "gl_api.hpp"

/*
 *  Functions I need:
 *  glGenTextures
 *  glBindTexture
 *  glActiveTexture
 *  glIsTexture
 *  glDeleteTextures
 *  glTexImage1D, glTexImage2D, glTexImage3D,
 *  glTexImage2DMultisample, glTexImage3DMultisample
 *  glTexSubImage2D, glTexSubImage3D
 */

/* TODO: Implement correct type handling for pixel formats */

zap::engine::gl::GLenum gl_internal_format(zap::engine::pixel_format format, zap::engine::pixel_datatype datatype) {
    using namespace zap::engine; using namespace gl;
    if(datatype == pixel_datatype::PD_UNSIGNED_BYTE) return gl_type(format);
    else if(datatype == pixel_datatype::PD_UNSIGNED_BYTE_3_3_2) return gl_type(format);
    else if(datatype == pixel_datatype::PD_FLOAT) {
        if(format == pixel_format::PF_RGB) return GL_RGB32F;
        else return GL_NONE;
    }
    return GL_NONE;
}

/* TODO: Fix all this code and build a proper set of reflection structures to automate binding. */

template <typename Pixel>
bool zap::engine::texture::initialise(size_t width, size_t height, const std::vector<Pixel>& buffer, bool generate_mipmaps) {
    using namespace gl;

    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, generate_mipmaps ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    const auto format =  gl_type(pixel_type<Pixel>::format);
    const auto datatype = gl_type(pixel_type<Pixel>::datatype);

    // TODO: Fix texture alignment (let float use 4 for now)
    int pixel_alignment = 0;
    if(pixel_type<Pixel>::datatype == pixel_datatype::PD_UNSIGNED_BYTE) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixel_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, datatype, buffer.data());
    if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    w_ = width; h_ = height; d_ = 1;

    release();
    if(pixel_alignment) glPixelStorei(GL_UNPACK_ALIGNMENT, pixel_alignment);
    return !gl_error_check();
}

template <typename PixelT, zap::engine::buffer_usage USAGE>
bool zap::engine::texture::initialise(const pixel_buffer<PixelT,USAGE>& pixbuf, bool generate_mipmaps) {
    using namespace gl;

    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generate_mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, generate_mipmaps ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    const auto format =  gl_type(pixel_type<PixelT>::format);
    const auto datatype = gl_type(pixel_type<PixelT>::datatype);

    // TODO: Fix texture alignment (let float use 4 for now)
    int pixel_alignment = 0;
    if(pixel_type<PixelT>::datatype == pixel_datatype::PD_UNSIGNED_BYTE) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixel_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    pixbuf.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, format, pixbuf.width(), pixbuf.height(), 0, format, datatype, 0);
    pixbuf.release();

    if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    w_ = pixbuf.width(); h_ = pixbuf.height(); d_ = 1;

    release();
    if(pixel_alignment) glPixelStorei(GL_UNPACK_ALIGNMENT, pixel_alignment);
    return !gl_error_check();
};

// Explicitly instantiate so we can avoid regenerating these functions
namespace zap { namespace engine {
    template bool texture::initialise<rgb888_t>(size_t width, size_t height, const std::vector<rgb888_t>& buffer, bool generate_mipmaps);
    template bool texture::initialise<rgb332_t>(size_t width, size_t height, const std::vector<rgb332_t>& buffer, bool generate_mipmaps);
    template bool texture::initialise<rgb32f_t>(size_t width, size_t height, const std::vector<rgb32f_t>& buffer, bool generate_mipmaps);
    template bool texture::initialise<rgb888_t,buffer_usage::BU_DYNAMIC_DRAW>(const pixel_buffer<rgb888_t,buffer_usage::BU_DYNAMIC_DRAW>& pixbuf, bool generate_mipmaps);
}}

using namespace zap::engine;
using namespace zap::engine::gl;

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

size_t texture::query_max_units() {
    GLint units = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &units);
    return units;
}

void texture::bind(size_t unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    gl_error_check();
    glBindTexture(gl_type(type_), id_);
    gl_error_check();
}

void texture::release() const {
    glBindTexture(gl_type(type_), 0);
}

bool texture::is_bound() const {
    LOG_WARN("OpenGL query operation invoked", "Incomplete function invoked");
    GLint bound = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound);   // TODO: Generic for tex type
    return id_ == (resource_t)bound;
}

bool texture::initialise(texture_type type, size_t width, size_t height, pixel_format format, pixel_datatype datatype, const void* data) {
    using namespace gl;

    type_ = type;
    auto gltype = gl_type(type);

    bind();
    glTexParameteri(gltype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(gltype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(gltype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(gltype, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // TODO: Fix texture alignment (let float use 4 for now)
    int pixel_alignment = 0;
    if(datatype == pixel_datatype::PD_UNSIGNED_BYTE) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixel_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    if(type_ == texture_type::TT_TEX1D) {
        if(format == pixel_format::PF_RED && datatype == pixel_datatype::PD_DN_UNSIGNED_BYTE)
            glTexImage1D(gltype, 0, GL_R8UI, width, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, data);
        else
            glTexImage1D(gltype, 0, gl_type(format), width, 0, gl_type(format), gl_type(datatype), data);

    } else if(type_ == texture_type::TT_TEX2D) {
        if(datatype == pixel_datatype::PD_FLOAT)
            glTexImage2D(gltype, 0, GL_RGB32F, width, height, 0, GL_RGB, gl_type(datatype), data);
        else
            glTexImage2D(gltype, 0, gl_type(format), width, height, 0, gl_type(format), gl_type(datatype), data);
    } else {
        LOG_ERR("This function is incomplete and the texture you've just initialised isn't gonna work.");
    }

    w_ = width; h_ = height; d_ = 1;

    release();
    if(pixel_alignment) glPixelStorei(GL_UNPACK_ALIGNMENT, pixel_alignment);
    return !gl_error_check();
}

bool texture::copy(size_t col, size_t row, size_t width, size_t height, int level, bool update_mipmaps, pixel_format format,
        pixel_datatype datatype, const char* data) {
    using namespace gl;
    auto gltype = gl_type(type_);

    bind();
    // TODO: Fix texture alignment (let float use 4 for now)
    int pixel_alignment = 0;
    if(datatype == pixel_datatype::PD_UNSIGNED_BYTE) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixel_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    if(type_ == texture_type::TT_TEX2D) {
        glTexSubImage2D(gltype, level, col, row, width, height, gl_type(format), gl_type(datatype), data);
    } else {
        LOG_ERR("This function is incomplete and the texture you've just initialised isn't gonna work.");
    }

    release();
    if(pixel_alignment) glPixelStorei(GL_UNPACK_ALIGNMENT, pixel_alignment);
    return !gl_error_check();
}

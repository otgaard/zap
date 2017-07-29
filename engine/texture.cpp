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
    else if(datatype == pixel_datatype::PD_DN_UNSIGNED_BYTE) {
        if(format == pixel_format::PF_RED_INTEGER)  return GL_R8UI;
        if(format == pixel_format::PF_RG_INTEGER)   return GL_RG8UI;
        if(format == pixel_format::PF_RGB_INTEGER)  return GL_RGB8UI;
        if(format == pixel_format::PF_RGBA_INTEGER) return GL_RGBA8UI;
    }
    else if(datatype == pixel_datatype::PD_FLOAT) {
        if(format == pixel_format::PF_RED)  return GL_R32F;
        if(format == pixel_format::PF_RG)   return GL_RG32F;
        if(format == pixel_format::PF_RGB)  return GL_RGB32F;
        if(format == pixel_format::PF_RGBA) return GL_RGBA32F;
    }
    return GL_NONE;
}

/* TODO: Fix all this code and build a proper set of reflection structures to automate binding. */

template <typename Pixel>
bool zap::engine::texture::initialise(size_t width, size_t height, const std::vector<Pixel>& buffer, bool generate_mipmaps) {
    using namespace gl;

    bind();
    initialise_default();

    const auto format =  gl_type(pixel_type<Pixel>::format);
    const auto datatype = gl_type(pixel_type<Pixel>::datatype);

    // TODO: Fix texture alignment (let float use 4 for now)
    int pixel_alignment = 0;
    if(pixel_type<Pixel>::datatype == pixel_datatype::PD_UNSIGNED_BYTE) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixel_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    const auto internal_fmt = gl_internal_format(pixel_type<Pixel>::format, pixel_type<Pixel>::datatype);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height, 0, format, datatype, buffer.data());
    if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    w_ = width; h_ = height; d_ = 1;

    release();
    if(pixel_alignment) glPixelStorei(GL_UNPACK_ALIGNMENT, pixel_alignment);
    return !gl_error_check();
}

template <typename PixelT>
bool zap::engine::texture::initialise(const pixel_buffer<PixelT>& pixbuf, bool generate_mipmaps) {
    using namespace gl;

    bind();
    initialise_default();

    const auto format =  gl_type(pixel_type<PixelT>::format);
    const auto datatype = gl_type(pixel_type<PixelT>::datatype);

    // TODO: Fix texture alignment (let float use 4 for now)
    int pixel_alignment = 0;
    if(pixel_type<PixelT>::datatype == pixel_datatype::PD_UNSIGNED_BYTE) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixel_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    const auto internal_fmt = gl_internal_format(pixel_type<PixelT>::format, pixel_type<PixelT>::datatype);

    pixbuf.bind();
    glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, pixbuf.width(), pixbuf.height(), 0, format, datatype, 0);
    pixbuf.release();

    if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);

    w_ = pixbuf.width(); h_ = pixbuf.height(); d_ = 1;

    release();
    if(pixel_alignment) glPixelStorei(GL_UNPACK_ALIGNMENT, pixel_alignment);
    return !gl_error_check();
};

template <typename PixelT>
bool zap::engine::texture::initialise(const pixmap<PixelT>& pmap, bool generate_mipmaps) {
    using namespace gl;

    bind();
    initialise_default();

    const auto format =  gl_type(pixel_type<PixelT>::format);
    const auto datatype = gl_type(pixel_type<PixelT>::datatype);

    // TODO: Fix texture alignment (let float use 4 for now)
    int pixel_alignment = 0;
    if(pixel_type<PixelT>::datatype == pixel_datatype::PD_UNSIGNED_BYTE) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixel_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    const auto internal_fmt = gl_internal_format(pixel_type<PixelT>::format, pixel_type<PixelT>::datatype);
    if(type_ == texture_type::TT_TEX1D) {
        glTexImage1D(GL_TEXTURE_1D, 0, internal_fmt, pmap.width(), 0, format, datatype, pmap.data());
        if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_1D);
    } else if(type_ == texture_type::TT_TEX2D) {
        glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, pmap.width(), pmap.height(), 0, format, datatype, pmap.data());
        if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_2D);
    } else if(type_ == texture_type::TT_TEX3D) {
        glTexImage3D(GL_TEXTURE_3D, 0, internal_fmt, pmap.width(), pmap.height(), pmap.depth(), 0, format, datatype, pmap.data());
        if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_3D);
    } else if(type_ == texture_type::TT_CUBE_MAP) {
        assert(pmap.depth() == 6 && "Cube Map must consist of six 2D textures stored in depth");
        for(int i = 0; i != 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, internal_fmt, pmap.width(), pmap.height(), 0,
                         format, datatype, pmap.data(0,0,i));
            gl_error_check();
        }

        if(generate_mipmaps) glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    w_ = pmap.width(); h_ = pmap.height(); d_ = 1;

    release();
    if(pixel_alignment) glPixelStorei(GL_UNPACK_ALIGNMENT, pixel_alignment);
    return !gl_error_check();
}

// Explicitly instantiate so we can avoid regenerating these functions
namespace zap { namespace engine {
    template bool texture::initialise<rgb888_t>(size_t width, size_t height, const std::vector<rgb888_t>& buffer, bool generate_mipmaps);
    template bool texture::initialise<rgba8888_t>(size_t width, size_t height, const std::vector<rgba8888_t>& buffer, bool generate_mipmaps);
    template bool texture::initialise<rgb332_t>(size_t width, size_t height, const std::vector<rgb332_t>& buffer, bool generate_mipmaps);
    template bool texture::initialise<r32f_t>(size_t width, size_t height, const std::vector<r32f_t>& buffer, bool generate_mipmaps);
    template bool texture::initialise<rgb32f_t>(size_t width, size_t height, const std::vector<rgb32f_t>& buffer, bool generate_mipmaps);
    template bool texture::initialise<rgba32f_t>(size_t width, size_t height, const std::vector<rgba32f_t>& buffer, bool generate_mipmaps);

    template bool texture::initialise<rgb888_t>(const pixmap<rgb888_t>& pmap, bool generate_mipmaps);
    template bool texture::initialise<rgba8888_t>(const pixmap<rgba8888_t>& pmap, bool generate_mipmaps);
    template bool texture::initialise<rgb332_t>(const pixmap<rgb332_t>& pmap, bool generate_mipmaps);
    template bool texture::initialise<r32f_t>(const pixmap<r32f_t>& pmap, bool generate_mipmaps);
    template bool texture::initialise<rgb32f_t>(const pixmap<rgb32f_t>& pmap, bool generate_mipmaps);
    template bool texture::initialise<rgba32f_t>(const pixmap<rgba32f_t>& pmap, bool generate_mipmaps);

    template bool texture::initialise<rgb888_t>(const pixel_buffer<rgb888_t>& pixbuf, bool generate_mipmaps);
    template bool texture::initialise<rgba8888_t>(const pixel_buffer<rgba8888_t>& pixbuf, bool generate_mipmaps);
    template bool texture::initialise<rgb332_t>(const pixel_buffer<rgb332_t>& pixbuf, bool generate_mipmaps);
    template bool texture::initialise<r32f_t>(const pixel_buffer<r32f_t>& pixbuf, bool generate_mipmaps);
    template bool texture::initialise<rgb32f_t>(const pixel_buffer<rgb32f_t>& pixbuf, bool generate_mipmaps);
    template bool texture::initialise<rgba32f_t>(const pixel_buffer<rgba32f_t>& pixbuf, bool generate_mipmaps);
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
    initialise_default();

    // TODO: Fix texture alignment (let float use 4 for now)
    int pixel_alignment = 0;
    if(datatype == pixel_datatype::PD_UNSIGNED_BYTE) {
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &pixel_alignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    const auto internal_fmt = gl_internal_format(format, datatype);
    const auto gl_format = gl_type(format);
    const auto gl_datatype = gl_type(datatype);

    if(type_ == texture_type::TT_TEX1D) glTexImage1D(gltype, 0, internal_fmt, width, 0, gl_format, gl_datatype, data);
    else if(type_ == texture_type::TT_TEX2D) glTexImage2D(gltype, 0, internal_fmt, width, height, 0, gl_format, gl_datatype, data);

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

void texture::initialise_default() {
    auto gltype = gl_type(type_);
    glTexParameteri(gltype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(gltype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(gltype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    if(is_one_of(type_, {texture_type::TT_TEX2D, texture_type::TT_TEX3D, texture_type::TT_CUBE_MAP}))
        glTexParameteri(gltype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if(is_one_of(type_, {texture_type::TT_TEX3D, texture_type::TT_CUBE_MAP}))
        glTexParameteri(gltype, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

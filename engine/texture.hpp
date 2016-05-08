/* Created by Darren Otgaar on 2016/05/07. http://www.github.com/otgaard/zap */
#ifndef ZAP_TEXTURE_HPP
#define ZAP_TEXTURE_HPP

#include <vector>
#include "engine.hpp"
#include "pixel_format.hpp"

namespace zap { namespace engine {

enum class texture_type : char {
    TT_NONE,
    TT_TEX1D,
    TT_TEX2D,
    TT_TEX2D_MS,
    TT_TEX3D_MS,
    TT_CUBE,
    TT_GUARD
};

enum class channels : char {
    CH_NONE,
    CH_RED,
    CH_RG,
    CH_RGB,
    CH_BGR,
    CH_RGBA,
    CH_BGRA,
    CH_DEPTH_COMPONENT,
    CH_DEPTH_STENCIL,
    CH_GUARD
};

class texture {
public:
    texture() : id_(INVALID_RESOURCE) { }
    ~texture();

    bool allocate();
    bool deallocate();
    bool is_allocated() const { return id_ != INVALID_RESOURCE; }

    void bind() const;
    void release() const;
    bool is_bound() const;

    // Test Function
    bool initialise(size_t width, size_t height, const std::vector<rgb332_t>& buffer, bool generate_mipmaps=true);

protected:
    resource_t id_;
};

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


}}

#endif //ZAP_TEXTURE_HPP

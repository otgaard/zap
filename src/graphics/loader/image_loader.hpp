//
// Created by Darren Otgaar on 2017/09/16.
//

#ifndef ZAP_IMAGE_LOADER_HPP
#define ZAP_IMAGE_LOADER_HPP

#include <string>
#include "stb_image.h"
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>

namespace zap { namespace loader {

template <typename PixelT>
zap::engine::pixmap<PixelT> load_image2D(const std::string& path) {
    using namespace zap::engine;

    int x, y, components;
    byte* data = stbi_load(path.c_str(), &x, &y, &components, PixelT::size);
    if(data != nullptr) {
        pixmap<PixelT> img{x, y};
        if(!img.copy(data, x*y*PixelT::size)) LOG_ERR("Failed to initialise pixmap");
        stbi_image_free(data);
        return img;
    }

    return pixmap<PixelT>{};
}

template <typename PixelT>
zap::engine::texture load_texture2D(const std::string& path, bool generate_mipmaps=false, bool flip_y=false) {
    using namespace zap::engine;

    auto img = load_image2D<PixelT>(path);
    if(img.size() > 0) {
        if(flip_y) img.flip_y();
        texture tex{texture_type::TT_TEX2D};
        if(!tex.allocate() || !tex.initialise(img, generate_mipmaps)) LOG_ERR("Failed to initialise texture");
        return tex;
    }

    return texture{};
}

}}

#endif //ZAP_IMAGE_LOADER_HPP

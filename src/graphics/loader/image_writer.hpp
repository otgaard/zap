//
// Created by otgaard on 2018/03/28.
//

#ifndef ZAP_IMAGE_WRITER_HPP
#define ZAP_IMAGE_WRITER_HPP

#include <string>
#include "stb_image_write.h"
#include <engine/pixmap.hpp>
#include <engine/texture.hpp>

namespace zap { namespace loader {

template <typename PixelT>
bool save_image2D(const engine::pixmap<PixelT>& img, const std::string& path);

template <>
inline bool save_image2D<engine::r8_t>(const engine::pixmap<engine::r8_t>& img, const std::string& path) {
    return 0 != stbi_write_png(path.c_str(), img.width(), img.height(), 1, img.data(), int(img.width()*engine::r8_t::bytesize));
}

template <>
inline bool save_image2D<engine::rgb888_t>(const engine::pixmap<engine::rgb888_t>& img, const std::string& path) {
    return 0 != stbi_write_png(path.c_str(), img.width(), img.height(), 3, img.data(), int(img.width()*engine::rgb888_t::bytesize));
}

template <>
inline bool save_image2D<engine::rgba8888_t>(const engine::pixmap<engine::rgba8888_t>& img, const std::string& path) {
    return 0 != stbi_write_png(path.c_str(), img.width(), img.height(), 4, img.data(), int(img.width()*engine::rgba8888_t::bytesize));
}

}}

#endif //ZAP_IMAGE_WRITER_HPP

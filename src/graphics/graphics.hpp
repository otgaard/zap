//
// Created by otgaard on 2018/02/11.
//

#ifndef ZAP_GRAPHICS_HPP
#define ZAP_GRAPHICS_HPP

#if defined(_WIN32)

#if !defined(GRAPHICS_EXPORT)
#if defined(ZAP_STATIC)
#include "graphics_exports_s.h"
#else
#include "graphics_exports.h"
#endif
#else
#include GRAPHICS_EXPORT
#endif

#else
#define ZAPGRAPHICS_EXPORT
#endif

#endif //ZAP_GRAPHICS_HPP

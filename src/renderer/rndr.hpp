//
// Created by otgaard on 2018/02/11.
//

#ifndef ZAP_RNDR_HPP
#define ZAP_RNDR_HPP

#if defined(_WIN32)

#if !defined(RENDERER_EXPORT)
#if defined(ZAP_STATIC)
#include "renderer_exports_s.h"
#else
#include "renderer_exports.h"
#endif
#else
#include RENDERER_EXPORT
#endif

#else
#define ZAPRENDERER_EXPORT
#endif

#endif //ZAP_RNDR_HPP

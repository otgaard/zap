//
// Created by Darren Otgaar on 2016/04/30.
//

#ifndef ZAP_PIXEL_FORMAT_HPP
#define ZAP_PIXEL_FORMAT_HPP

#include <maths/maths.hpp>

// The pixel_format class allows packing a pixel into a configurable space and supports signed, unsigned, packed,
// integer, and floating-point types

namespace zap { namespace engine {
    using zap::maths::pow2;

    // Allowable data types
    // (U)BYTE, (U)SHORT, (U)INT, HALF_FLOAT, FLOAT
    // 1 byte, 2 bytes, 4 bytes, 2 bytes, 4 bytes

    enum class channel_type : byte {
        CT_NONE = 0,
        CT_ALPHA = 1,
        CT_LUMINANCE = 2,
        CT_RED = 3,
        CT_GREEN = 4,
        CT_BLUE = 5,
        CT_DEPTH = 6,
        CT_STENCIL = 7
    };

    template <typename... Args>
    struct pixel_format {
    };


}}

#endif //ZAP_PIXEL_FORMAT_HPP

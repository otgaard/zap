//
// Created by Darren Otgaar on 2017/07/22.
//

#ifndef ZAP_PIXEL_CONVERSION_HPP
#define ZAP_PIXEL_CONVERSION_HPP

#include <engine/pixel_format.hpp>

namespace zap { namespace engine {

template <typename PixelT> class pixmap;

template <typename PixelA, typename PixelB> PixelB convert(const PixelA& A);

template<typename Input, typename Output>
bool convert(const engine::pixmap<Input>& input, engine::pixmap<Output>& output);

template<>
bool convert<float, engine::rgb888_t>(const engine::pixmap<float>& input, engine::pixmap<engine::rgb888_t>& output);

template <>
inline engine::rgb888_t convert<engine::r8_t, engine::rgb888_t>(const engine::r8_t& A) {
    return engine::rgb888_t{A.get1(), A.get1(), A.get1()};
};

}}

#endif //ZAP_PIXEL_CONVERSION_HPP

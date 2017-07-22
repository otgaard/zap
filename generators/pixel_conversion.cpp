//
// Created by Darren Otgaar on 2017/07/22.
//

#include "pixel_conversion.hpp"

using namespace zap;

template<>
bool zap::convert(const engine::pixmap<float>& input, engine::pixmap<engine::rgb888_t>& output) {
    assert(input.size() >= output.size() && "Output size must be >= to Input size");
    if(input.size() < output.size()) return false;

    const float scale = 128.f, bias = 127.f;
    for(auto px = 0; px != input.size(); ++px) output[px].set((byte)(scale*input[px] + bias));
    return true;
}


//
// Created by Darren Otgaar on 2016/04/07.
//

#include <gtest/gtest.h>
#include <maths/maths.hpp>
#include <engine/engine.hpp>
#include <engine/buffer_format.hpp>

using namespace zap;
using namespace zap::engine;

// The heart of the vertex buffer is the pod.  The pod contains multiple, interleaved attributes.

TEST(ZapEngineBufferFormatTests, TypeTests) {
    static_assert(maths::pow2<10>::value == 2*2*2*2*2*2*2*2*2*2, "Whoa");
}

TEST(ZapEngineBufferFormatTests, PODTests) {

}
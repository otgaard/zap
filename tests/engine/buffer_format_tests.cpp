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

    static_assert(size_t(buffer_access::bitfield::BA_GUARD) == maths::pow2<6>::value, "BA_GUARD must be the last value in buffer_access");
    static_assert(buffer_access::BA_SIZE == maths::log2_pow2(size_t(buffer_access::bitfield::BA_GUARD)),
                  "BA_SIZE must be set to the number of bits used in the bitfield");
    constexpr auto last = maths::pow2<6>::value;
    constexpr auto size = maths::log2_pow2(size_t(buffer_access::bitfield::BA_GUARD));
    ASSERT_EQ(size_t(buffer_access::bitfield::BA_GUARD), last);
    constexpr auto val = buffer_access::BA_SIZE; // not expanding correctly
    ASSERT_EQ(val, size);
}

TEST(ZapEngineBufferFormatTests, PODTests) {

}
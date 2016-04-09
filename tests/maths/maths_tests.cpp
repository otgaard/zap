//
// Created by Darren Otgaar on 2016/03/12.
//

#include <gtest/gtest.h>
#include <maths/maths.hpp>

using namespace zap::maths;

TEST(ZapMathsTests, ConstexprSqrt) {
    constexpr double csqrt2 = csqrt(2.0);
    constexpr double epsilon = std::numeric_limits<float>::epsilon(); // At least float precision

    EXPECT_LE(std::abs(1.41421356237 - csqrt2), epsilon);
    static_assert(abs(1.41421356237 - csqrt2) < epsilon, "Constexpr definition of square root has failed");
}

TEST(ZapMathsTests, Clamp) {
    constexpr float clamp1 = clamp(10.f);
    constexpr float clamp2 = clamp(-2.f);
    static_assert(clamp1 == 1.f, "max clamp to unit space failed");
    static_assert(clamp2 == 0.f, "min clamp to unit space failed");
    constexpr float clamp3 = clamp(2.f, -5.f, 5.f);
    constexpr float clamp4 = clamp(10.f, -5.f, 5.f);
    constexpr float clamp5 = clamp(-10.f, -5.f, 5.f);
    static_assert(clamp3 == 2.f, "expected unclamped value");
    static_assert(clamp4 == 5.f, "max clamp to space failed");
    static_assert(clamp5 == -5.f, "min clamp to space failed");
    EXPECT_EQ(clamp1 + clamp2 + clamp3 + clamp4 + clamp5, 1.f + 0.f + 2.f + 5.f - 5.f);
}

TEST(ZapMathsTests, Log2Power2) {
    static_assert(0 == log2_pow2(1 << 0), "log2_power2 failed on 1 << 0");
    static_assert(5 == log2_pow2(1 << 5), "log2_power2 failed 1 << 5");
    static_assert(10 == log2_pow2(1 << 10), "log2_power2 failed 1 << 10");
    EXPECT_EQ(log2_pow2(1 << 5), 5);
}

TEST(ZapMathsTests, Power2) {
    static_assert(1 == pow2<0>::value, "pow2(0) should equal 1");
    static_assert(2*2*2*2*2 == pow2<5>::value, "pow2(5) should equal 2*2*2*2*2");
    constexpr auto two_to_the_seven = pow2<7>::value;   // EXPECT_EQ causes a link error due to expansion
    EXPECT_EQ(2*2*2*2*2*2*2, two_to_the_seven);
}
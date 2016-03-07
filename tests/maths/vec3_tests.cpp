/* Created by Darren Otgaar on 2016/03/07. http://www.github.com/otgaard/zap */
#include <gtest/gtest.h>
#include <maths/vec3.hpp>

using namespace zap::maths;
using vec3f = vec3<float>;

TEST(ZapMathsTestsVec3, ConstexprTest) {
    constexpr vec3f u = {10.f, 20.f, 30.f};
    constexpr vec3f v = {1.f, 2.f, 3.f};
    constexpr vec3f expr = 2.f*u + v;

    static_assert(u.x == 10.f && u.y == 20.f && u.z == 30.f, "vec3f u is not a valid constexpr");
    static_assert(v.x == 1.f && v.y == 2.f && v.z == 3.f, "vec3f v is not a valid constexpr");

    static_assert(expr.x == 21.f && expr.y == 42.f && expr.z == 63.f, "vec3f expr is not a valid constexpr");

    EXPECT_EQ(v.x, 1.f);
    EXPECT_EQ(v.y, 2.f);
    EXPECT_EQ(v.z, 3.f);

    EXPECT_EQ(u.x, 10.f);
    EXPECT_EQ(u.y, 20.f);
    EXPECT_EQ(u.z, 30.f);

    EXPECT_EQ(expr.x, 21.f);
    EXPECT_EQ(expr.y, 42.f);
    EXPECT_EQ(expr.z, 63.f);
}

TEST(ZapMathsTestsVec3, AlgebraicTest) {
    vec3f u = {10.f, 20.f, 30.f};
    vec3f v = {1.f, 2.f, 3.f};
    vec3f w = {-5.f, -10.f, -15.f};

    vec3f r1 = u + v + w;
    EXPECT_EQ(r1.x, 6.f);
    EXPECT_EQ(r1.y, 12.f);
    EXPECT_EQ(r1.z, 18.f);

    float r2 = dot(u,u);
    EXPECT_EQ(r2, u.x*u.x + u.y*u.y + u.z*u.z);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

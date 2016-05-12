//
// Created by Darren Otgaar on 2016/05/10.
//

#include <gtest/gtest.h>
#include <maths/mat2.hpp>

using namespace zap::maths;

TEST(ZapMat2Tests, ConstexprInitialisation) {
    // Tests various constexpr paths

    static_assert(mat2<float>::bytesize() == sizeof(float)*4, "Constexpr bytesize failed");
    ASSERT_EQ(mat2<float>(1.f,-1.f)(0,0), 1.f);
    ASSERT_EQ(mat2<float>(1.f,-1.f)(0,1), 0.f);
    ASSERT_EQ(mat2<float>(1.f,-1.f)(1,0), 0.f);
    ASSERT_EQ(mat2<float>(1.f,-1.f)(1,1), -1.f);

    constexpr mat2<float> m1 = {1,2,3,4};
    static_assert(m1(0,0) == 1, "mat2<float> not constexpr");
    static_assert(m1(0,1) == 2, "mat2<float> not constexpr");
    static_assert(m1(1,0) == 3, "mat2<float> not constexpr");
    static_assert(m1(1,1) == 4, "mat2<float> not constexpr");
}

TEST(ZapMat2Tests, ColumnMajorConstructor) {
    mat2<float> m1 = { 1, 2, 3, 4 };
    ASSERT_EQ(m1(0,0), 1);
    ASSERT_EQ(m1(0,1), 2);
    ASSERT_EQ(m1(1,0), 3);
    ASSERT_EQ(m1(1,1), 4);
}

TEST(ZapMat2Tests, Indexing) {
    mat2<float> m1 = { 1, 2, 3, 4 };
    ASSERT_EQ(m1(0,0), m1[0]);
    ASSERT_EQ(m1(0,1), m1[2]);
    ASSERT_EQ(m1(1,0), m1[1]);
    ASSERT_EQ(m1(1,1), m1[3]);
}

TEST(ZapMat2Tests, Operators) {
    using mat2i = mat2<int>;
    constexpr mat2i m1 = { 1, 2,
                           3, 4 };
    constexpr mat2i m2 = { -1, 2,
                           -3, 4 };

    constexpr mat2i m3 = m1 + m2;
    constexpr mat2i m4 = m1 - m2;
    constexpr mat2i m5 = m1 * m2;

    ASSERT_EQ(m3(0,0), 0);
    static_assert(m3(0,0) == 0, "mat2i not constexpr");

    ASSERT_EQ(m4(0,0), 2);
    static_assert(m4(0,0) == 2, "mat2i not constexpr");

    ASSERT_EQ(m5(0,0), -7);
    static_assert(m5(0,0) == -7, "mat2i not constexpr");
}
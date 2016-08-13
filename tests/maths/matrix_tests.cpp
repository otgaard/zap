//
// Created by Darren Otgaar on 2016/03/15.
//

#include <gtest/gtest.h>
#include <maths/mat2.hpp>
#include <maths/mat3.hpp>
#include <maths/mat4.hpp>

#define LOGGING_ENABLED
#include <tools/log.hpp>

using namespace zap::maths;

TEST(MatrixMathsTests, Mat2Tests) {
    using mat_t = mat2<float>;
    constexpr mat_t mat1 = { 1, 2,
                             3, 4 };
    auto col0 = mat1.col(0);
    EXPECT_EQ(col0[0], 1); EXPECT_EQ(col0[1], 3);
    auto row0 = mat1.row(0);
    EXPECT_EQ(row0[0], 1); EXPECT_EQ(row0[1], 2);
    auto col1 = mat1.col(1);
    EXPECT_EQ(col1[0], 2); EXPECT_EQ(col1[1], 4);
    auto row1 = mat1.row(1);
    EXPECT_EQ(row1[0], 3); EXPECT_EQ(row1[1], 4);

    constexpr mat_t mat2 = { -1,  2,
                              3, -4 };
    auto r1 = mat1 + mat2;
    EXPECT_EQ(r1(0,0), 0); EXPECT_EQ(r1(0,1), 4);
    EXPECT_EQ(r1(1,0), 6); EXPECT_EQ(r1(1,1), 0);

    //constexpr auto r2 = mat1 * mat2;
    /*
    static_assert(r2(0,0) == 5, "Error");
    static_assert(r2(1,0) == 9, "Error");
    static_assert(r2(0,1) == -6, "Error");
    static_assert(r2(1,1) == -10, "Error");
    */

    //constexpr auto t2 = transpose(r2);
    /*
    static_assert(t2(0,0) == 5, "Error");
    static_assert(t2(1,0) == -6, "Error");
    static_assert(t2(0,1) == 9, "Error");
    static_assert(t2(1,1) == -10, "Error");
    */

    auto r3 = mat_t::make_rotation(PI);
    for(const auto& v : r3) LOG(v);
}

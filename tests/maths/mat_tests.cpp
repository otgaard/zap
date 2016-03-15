//
// Created by Darren Otgaar on 2016/03/15.
//

#include <gtest/gtest.h>
#include <maths/mat2.hpp>

using namespace zap::maths;

TEST(MatrixMathsTests, ColumnMajorStorage) {
    mat2<float> M1 = {{1.f, 2.f, 3.f, 4.f}};
    mat2<float> M2 = {{1.f, 2.f},
                      {3.f, 4.f}};
    mat2<float> M3 = {1.f, 2.f, 3.f, 4.f};

    EXPECT_EQ(M1(0,0), M2(0,0));
    EXPECT_EQ(M1(1,0), M2(1,0));
    EXPECT_EQ(M1(0,1), M2(0,1));
    EXPECT_EQ(M1(1,1), M2(1,1));

    EXPECT_EQ(M2(0,0), M3(0,0));
    EXPECT_EQ(M2(1,0), M3(1,0));
    EXPECT_EQ(M2(0,1), M3(0,1));
    EXPECT_EQ(M2(1,1), M3(1,1));

    
}

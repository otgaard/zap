//
// Created by Darren Otgaar on 2016/03/15.
//

#include <gtest/gtest.h>
#include <maths/mat2.hpp>
#include <maths/mat3.hpp>
#include <maths/mat4.hpp>

using namespace zap::maths;

TEST(MatrixMathsTests, ColumnMajorStorageMat2) {
    mat2<float> M1 = {{1.f, 2.f, 3.f, 4.f}};
    mat2<float> M2 = {{1.f, 3.f},               // Column Initialiser
                      {2.f, 4.f}};
    mat2<float> M3 = {1.f, 2.f, 3.f, 4.f};

    EXPECT_EQ(M1(0,0), M2(0,0)); EXPECT_EQ(M2(0,0), M3(0,0));
    EXPECT_EQ(M1(1,0), M2(1,0)); EXPECT_EQ(M2(1,0), M3(1,0));
    EXPECT_EQ(M1(0,1), M2(0,1)); EXPECT_EQ(M2(0,1), M3(0,1));
    EXPECT_EQ(M1(1,1), M2(1,1)); EXPECT_EQ(M2(1,1), M3(1,1));
}

TEST(MatrixMathsTests, ConstructorsMat2) {
    mat2<float> M1(0.f);
    mat2<float> M2(3.f, 9.f);

}

TEST(MatrixMathsTests, ColumnMajorStorageMat3) {
    mat3<float> M1 = {{1.f, 2.f, 3.f,
                       4.f, 5.f, 6.f,
                       7.f, 8.f, 9.f}};
    mat3<float> M2 = {{1.f, 4.f, 7.f},          // Note, column initialiser
                      {2.f, 5.f, 8.f},
                      {3.f, 6.f, 9.f}};
    mat3<float> M3 = {1.f, 2.f, 3.f,
                      4.f, 5.f, 6.f,
                      7.f, 8.f, 9.f};

    EXPECT_EQ(M1(0,0), M2(0,0)); EXPECT_EQ(M2(0,0), M3(0,0));
    EXPECT_EQ(M1(0,1), M2(0,1)); EXPECT_EQ(M2(0,1), M3(0,1));
    EXPECT_EQ(M1(0,2), M2(0,2)); EXPECT_EQ(M2(0,2), M3(0,2));
    EXPECT_EQ(M1(1,0), M2(1,0)); EXPECT_EQ(M2(1,0), M3(1,0));
    EXPECT_EQ(M1(1,1), M2(1,1)); EXPECT_EQ(M2(1,1), M3(1,1));
    EXPECT_EQ(M1(1,2), M2(1,2)); EXPECT_EQ(M2(1,2), M3(1,2));
    EXPECT_EQ(M1(2,0), M2(2,0)); EXPECT_EQ(M2(2,0), M3(2,0));
    EXPECT_EQ(M1(2,1), M2(2,1)); EXPECT_EQ(M2(2,1), M3(2,1));
    EXPECT_EQ(M1(2,2), M2(2,2)); EXPECT_EQ(M2(2,2), M3(2,2));
}

TEST(MatrixMathsTests, ColumnMajorStorageMat4) {
    mat4<float> M1 = {{1.f,  2.f,  3.f,  4.f,
                       5.f,  6.f,  7.f,  8.f,
                       9.f, 10.f, 11.f, 12.f,
                      13.f, 14.f, 15.f, 16.f}};
    mat4<float> M2 = {{1.f,  5.f,  9.f, 13.f},          // Note, column initialiser
                      {2.f,  6.f, 10.f, 14.f},
                      {3.f,  7.f, 11.f, 15.f},
                      {4.f,  8.f, 12.f, 16.f}};
    mat4<float> M3 = {1.f,  2.f,  3.f,  4.f,
                      5.f,  6.f,  7.f,  8.f,
                      9.f, 10.f, 11.f, 12.f,
                     13.f, 14.f, 15.f, 16.f};

    EXPECT_EQ(M1(0,0), M2(0,0)); EXPECT_EQ(M2(0,0), M3(0,0));
    EXPECT_EQ(M1(0,1), M2(0,1)); EXPECT_EQ(M2(0,1), M3(0,1));
    EXPECT_EQ(M1(0,2), M2(0,2)); EXPECT_EQ(M2(0,2), M3(0,2));
    EXPECT_EQ(M1(0,3), M2(0,3)); EXPECT_EQ(M2(0,3), M3(0,3));
    EXPECT_EQ(M1(1,0), M2(1,0)); EXPECT_EQ(M2(1,0), M3(1,0));
    EXPECT_EQ(M1(1,1), M2(1,1)); EXPECT_EQ(M2(1,1), M3(1,1));
    EXPECT_EQ(M1(1,2), M2(1,2)); EXPECT_EQ(M2(1,2), M3(1,2));
    EXPECT_EQ(M1(1,3), M2(1,3)); EXPECT_EQ(M2(1,3), M3(1,3));
    EXPECT_EQ(M1(2,0), M2(2,0)); EXPECT_EQ(M2(2,0), M3(2,0));
    EXPECT_EQ(M1(2,1), M2(2,1)); EXPECT_EQ(M2(2,1), M3(2,1));
    EXPECT_EQ(M1(2,2), M2(2,2)); EXPECT_EQ(M2(2,2), M3(2,2));
    EXPECT_EQ(M1(2,3), M2(2,3)); EXPECT_EQ(M2(2,3), M3(2,3));
    EXPECT_EQ(M1(3,0), M2(3,0)); EXPECT_EQ(M2(3,0), M3(3,0));
    EXPECT_EQ(M1(3,1), M2(3,1)); EXPECT_EQ(M2(3,1), M3(3,1));
    EXPECT_EQ(M1(3,2), M2(3,2)); EXPECT_EQ(M2(3,2), M3(3,2));
    EXPECT_EQ(M1(3,3), M2(3,3)); EXPECT_EQ(M2(3,3), M3(3,3));
}

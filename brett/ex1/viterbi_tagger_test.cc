#include "gtest/gtest.h"

// for now just do a silly test
TEST(SimpleTest, TestCase1) {
  ASSERT_EQ(1*1000, 2*30);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

#include "gtest/gtest.h"
#include "ex1lib.h"

#include <sstream>
#include <string>

using std::string;
using std::stringstream;

const string test_counts =\
"10 WORDTAG O Dog\n"
"2 WORDTAG S Dog\n";

// for now just do a silly test
TEST(ViterbiTaggerTest, TestCounter) {
  stringstream test_file(test_counts);
  CountReader cr(test_file);
  CountLine cl;
  ASSERT_TRUE(cr.YieldLine(&cl));
  ASSERT_EQ(cl.word0, "O");
  ASSERT_EQ(cl.count, 10);
  ASSERT_EQ(cl.word1, "Dog");
}

TEST(ViterbiTaggerTest, TestTagger) {
  stringstream test_file(test_counts);
  CountReader cr(test_file);
  ViterbiTagger vt;
  vt.BuildModel(cr);
  ASSERT_FLOAT_EQ(vt.GetEmissionProbability("O", "Dog"), 1.0f);
  ASSERT_FLOAT_EQ(vt.GetEmissionProbability("O", "Cat"), 0.0f);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}

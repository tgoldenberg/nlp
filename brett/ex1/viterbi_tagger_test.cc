#include "gtest/gtest.h"
#include "ex1lib.h"

#include <string>
#include <sstream>
#include <vector>

using std::set;
using std::string;
using std::stringstream;
using std::vector;

string word_count_data =
"1 WORDTAG O _RARE_\n"
"20 WORDTAG O resting\n"
"1 WORDTAG I-GENE _RARE_\n"
"2 WORDTAG I-GENE holoenzyme\n"
"2 WORDTAG I-GENE hydrolase\n"
"2 WORDTAG I-GENE barley\n"
"2 WORDTAG O glottic\n"
"24435 2-GRAM I-GENE I-GENE\n"
"15888 2-GRAM O I-GENE\n"
"16624 2-GRAM I-GENE O\n"
"113783 2-GRAM O STOP\n"
"13 2-GRAM I-GENE STOP\n"
"13047 2-GRAM * O\n"
"315457 2-GRAM O O\n"
"749 2-GRAM * I-GENE\n"
"13796 2-GRAM * *\n"
"749 3-GRAM * * I-GENE\n"
"11320 3-GRAM I-GENE O O\n"
"9622 3-GRAM I-GENE I-GENE O\n"
"291686 3-GRAM O O O\n"
"1 3-GRAM O I-GENE STOP\n"
"12451 3-GRAM * O O\n"
"3491 3-GRAM I-GENE O I-GENE\n"
"11 3-GRAM I-GENE I-GENE STOP\n"
"14802 3-GRAM I-GENE I-GENE I-GENE\n"
"3 3-GRAM * O STOP\n"
"296 3-GRAM * I-GENE O\n"
"11967 3-GRAM O O STOP\n"
"6706 3-GRAM O I-GENE O\n"
"9181 3-GRAM O I-GENE I-GENE\n"
"452 3-GRAM * I-GENE I-GENE\n"
"1813 3-GRAM I-GENE O STOP\n"
"13047 3-GRAM * * O\n"
"1 3-GRAM * I-GENE STOP\n"
"11804 3-GRAM O O I-GENE\n"
"593 3-GRAM * O I-GENE\n";

TEST(ViterbiTaggerTest, TestEmissionProbability) {
  stringstream test_file(word_count_data);
  CountReader cr(test_file);
  ViterbiTagger vt;
  vt.BuildModel(cr);

  EXPECT_GT(vt.GetEmissionProbability("O", "_RARE_"), 0);
  EXPECT_GT(vt.GetEmissionProbability("I-GENE", "_RARE_"), 0);
}

TEST(ViterbiTaggerTest, TestTrigramProbability) {
  stringstream test_file(word_count_data);
  CountReader cr(test_file);
  ViterbiTagger vt;
  vt.BuildModel(cr);
  EXPECT_FLOAT_EQ(vt.GetTrigramProbability("O", "O", "O"), 0.5);
  EXPECT_FLOAT_EQ(vt.GetTrigramProbability("O", "O", "STOP"), 0.5);
  EXPECT_FLOAT_EQ(vt.GetTrigramProbability("O", "O", "I-GENE"), 0.5);
  EXPECT_FLOAT_EQ(vt.GetTrigramProbability("O", "O", "*"), 0.5);
}

TEST(ViterbiTaggerTest, TestSampler) {
  stringstream test_file(word_count_data);
  CountReader cr(test_file);
  ViterbiTagger vt;
  vt.BuildModel(cr);

  vector<string> sentence = {"Dogs", "are", "cool", "wouldn't", "yah", "say"};

  ViterbiSolver vs(sentence, vt);
  EXPECT_EQ(vs.SampleSentence(-2), "*");
  EXPECT_EQ(vs.SampleSentence(-1), "*");
  EXPECT_EQ(vs.SampleSentence(0), "_RARE_");
  EXPECT_EQ(vs.SampleSentence(1), "_RARE_");
  EXPECT_EQ(vs.SampleSentence(2), "_RARE_");
  EXPECT_EQ(vs.SampleSentence(3), "_RARE_");
  EXPECT_EQ(vs.SampleSentence(4), "_RARE_");
  EXPECT_EQ(vs.SampleSentence(5), "_RARE_");
  EXPECT_EQ(vs.SampleSentence(6), "STOP");
}

TEST(ViterbiTaggerTest, TestSentence) {
  stringstream test_file(word_count_data);
  CountReader cr(test_file);
  ViterbiTagger vt;
  vt.BuildModel(cr);

  vector<string> sentence = {"Dogs", "are", "cool", "wouldn't", "yah", "say"};
  vector<TaggedWord> tagged_sentence = vt.TagSentence(sentence);

  EXPECT_EQ(tagged_sentence[0].tag, "O");
  EXPECT_EQ(tagged_sentence[1].tag, "O");
  EXPECT_EQ(tagged_sentence[2].tag, "O");
  EXPECT_EQ(tagged_sentence[3].tag, "O");
  EXPECT_EQ(tagged_sentence[4].tag, "O");
  EXPECT_EQ(tagged_sentence[5].tag, "O");
}

TEST(ViterbiTaggerTest, TestBackPointer) {
  stringstream test_file(word_count_data);
  CountReader cr(test_file);
  ViterbiTagger vt;
  vt.BuildModel(cr);

  vector<string> sentence = {"Dogs", "are", "cool", "wouldn't", "yah", "say"};
  ViterbiSolver vs(sentence, vt);
  vector<TaggedWord> tagged_sentence = vs.TagSentence();

  vs.DebugBackPointers();

  EXPECT_EQ(tagged_sentence[0].tag, "O");
  EXPECT_EQ(tagged_sentence[1].tag, "O");
  EXPECT_EQ(tagged_sentence[2].tag, "O");
  EXPECT_EQ(tagged_sentence[3].tag, "O");
  EXPECT_EQ(tagged_sentence[4].tag, "O");
  EXPECT_EQ(tagged_sentence[5].tag, "O");
}

TEST(ViterbiTaggerTest, TestPi) {
  stringstream test_file(word_count_data);
  CountReader cr(test_file);
  ViterbiTagger vt;
  vt.BuildModel(cr);

  vector<string> sentence = {"Dogs", "are", "cool", "wouldn't", "yah", "say"};
  ViterbiSolver vs(sentence, vt);
/*
  EXPECT_FLOAT_EQ(vs.Pi("*", "O", 1), 1.0f);
  EXPECT_FLOAT_EQ(vs.Pi("*", "I-GENE", 1), 1.0f);

  // Anything other than * * at 0 should have 0 probability.
  EXPECT_FLOAT_EQ(vs.Pi("*", "*", 0), 1.0f);
  EXPECT_FLOAT_EQ(vs.Pi("*", "O", 0), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("O", "O", 0), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("I-GENE", "O", 0), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("O", "*", 0), 0.0f);

  // Anything at position 1 should have * in the beginning.
  EXPECT_FLOAT_EQ(vs.Pi("O", "*", 1), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("O", "O", 1), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("O", "O", 1), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("I-GENE", "O", 1), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("O", "*", 1), 0.0f);

  EXPECT_GT(vs.Pi("*", "O", 1), 0.0f);
  EXPECT_GT(vs.Pi("*", "I-GENE", 1), 0.0f);

  EXPECT_GT(vs.Pi("I-GENE", "I-GENE", 2), 0.01f);
  */
  EXPECT_GT(vs.Pi("O", "O", 2), 0.0001f);
  vs.DebugBackPointers();
/*
  // Nothing after position 1 should have a non zero probability for *
  EXPECT_FLOAT_EQ(vs.Pi("*", "O", 2), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("*", "I-GENE", 2), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("*", "O", 2), 0.0f);
  EXPECT_FLOAT_EQ(vs.Pi("*", "I-GENE", 2), 0.0f);
  */
}

TEST(ViterbiTaggerTest, TestGetTags) {
  stringstream test_file(word_count_data);
  CountReader cr(test_file);
  ViterbiTagger vt;
  vt.BuildModel(cr);
  EXPECT_EQ(vt.GetTags(), set<string>({"O", "I-GENE"}));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int ret = RUN_ALL_TESTS();
  return ret;
}

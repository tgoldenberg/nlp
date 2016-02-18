#include "ex1lib.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

using std::vector;
using std::cout;
using std::string;

int main(int argc, char **argv) {
  std::ifstream input_file(argv[1]);
  CountReader count_reader(input_file);
  ViterbiTagger viterbi_tagger;
  viterbi_tagger.BuildModel(count_reader);

  std::ifstream data_file(argv[2]);
  DataReader data_reader(data_file);
  DataLine data_line;
  while (data_reader.YieldLine(&data_line)) {
    vector<string> sentence_to_tag;
    for (const TaggedWord& tagged_word : data_line.tagged_words) {
      sentence_to_tag.push_back(tagged_word.word);
    }
    for (const TaggedWord& tagged_word : viterbi_tagger.TagSentence(sentence_to_tag)) {
      cout << tagged_word.word << " " << (!tagged_word.tag.empty() ? tagged_word.tag : "_UNKNOWN_") << std::endl;
    }
    cout << std::endl;
    data_line.tagged_words.clear();
  }
  return 0;
}

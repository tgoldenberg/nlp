#include "ex1lib.h"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char **argv) {
  std::ifstream input_file(argv[1]);
  CountReader count_reader(input_file);
  SimpleTagger simple_tagger;
  simple_tagger.BuildModel(count_reader);

  std::ifstream data_file(argv[2]);
  DataReader data_reader(data_file);
  DataLine data_line;
  while (data_reader.YieldLine(&data_line)) {
    for (const TaggedWord& tagged_word : data_line.tagged_words) {
      std::cout << tagged_word.word << " " << simple_tagger.TagWord(tagged_word.word)
                << std::endl;
    }
    std::cout << std::endl;
    data_line.tagged_words.clear();
  }
  return 0;
}

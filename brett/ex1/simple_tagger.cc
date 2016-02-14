#include "ex1lib.h"

#include <fstream>
#include <iostream>
#include <sstream>

int main(int argc, char **argv) {
  std::ifstream input_file(argv[1]);
  ModelReader model_reader(input_file);
  SimpleTagger simple_tagger;
  simple_tagger.BuildModel(model_reader);

  std::ifstream test_file(argv[2]);
  std::string test_line;
  std::stringstream line_stream;
  while (std::getline(test_file, test_line)) {
    if (test_line.empty()) {
      std::cout << std::endl;
      continue;
    }
    line_stream.str(test_line);
    line_stream.clear();
    std::string line_word;
    line_stream >> line_word;
    std::cout << line_word << " " << simple_tagger.TagWord(line_word)
              << std::endl;
  }
  return 0;
}

#include "ex1lib.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

int main(int argc, char** argv) {
  std::ifstream input_file(argv[1]);

  CountReader count_reader(input_file);
  std::map<std::string, int> word_counts = get_word_counts(count_reader);
  for (const auto& it : word_counts) {
    if (it.second < 5) {
      std::cout << it.first << std::endl;
    }
  }

  return 0;
}

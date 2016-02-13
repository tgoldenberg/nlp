#include "ex1lib.h"

#include <iostream>
#include <string>
#include <unordered_map>

int main(int argc, char** argv) {
  const char* input_path = argv[1];
  std::unordered_map<std::string, int> word_counts = get_word_counts(input_path);
  for (const auto& it : word_counts) {
    if (it.second < 5) {
      std::cout << it.first << std::endl;
    }
  }
}

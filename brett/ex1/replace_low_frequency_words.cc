#include "ex1lib.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_set>

int main(int argc, char** argv) {
  std::ifstream set_file(argv[1]);
  std::unordered_set<std::string> replacements;
  
  // Fill the replacements set
  std::string set_line;
  while (std::getline(set_file, set_line)) {
    replacements.insert(set_line);   
  }
  set_file.close();

  // replace a worde with _RARE_
  // if it is in the replacement set.
  std::ifstream data_file(argv[2]);
  DataReader data_reader(data_file);
  DataLine data_line;
  while (data_reader.YieldLine(&data_line)) {
    for (const TaggedWord& tagged_word : data_line.tagged_words) {
      std::string out_word = tagged_word.word;
      if (replacements.find(tagged_word.word) != replacements.end()) {
        out_word = "_RARE_";
      }
      std::cout << out_word << " " << tagged_word.tag << std::endl;
    }
    std::cout << std::endl;
    data_line.tagged_words.clear();
  }
  return 0;
}

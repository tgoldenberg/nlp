#include "ex1lib.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

using std::ifstream;
using std::string;
using std::stringstream;
using std::unordered_map;

unordered_map<string, int> get_word_counts(const char* input_path) {
  unordered_map<std::string, int> word_counts;
  ifstream input_file(input_path);
  stringstream line_stream;
  string line;
  while (std::getline(input_file, line)) {
    line_stream.str(line);
    line_stream.clear();

    int line_count;
    string line_type;
    string line_tag;   
    string line_word;

    line_stream >> line_count;
    line_stream >> line_type;
    line_stream >> line_tag;
    line_stream >> line_word;

    if (line_type == "WORDTAG") {
      int existing_count = 0;
      if (word_counts.find(line_word) != word_counts.end()) {
        existing_count = word_counts[line_word];       
      }
      word_counts[line_word] = existing_count + line_count;
    }
  }

  return word_counts;
}


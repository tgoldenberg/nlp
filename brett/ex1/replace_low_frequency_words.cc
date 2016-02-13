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
  std::ifstream replace_file(argv[2]);
  std::string replace_line;
  std::stringstream line_stream;
  while (std::getline(replace_file, replace_line)) {
    if (replace_line.empty()) {
      std::cout << std::endl;
      continue;
    }
    line_stream.str(replace_line);
    line_stream.clear();
    std::string line_word;
    line_stream >> line_word;
    std::string out_word = line_word;
    if (replacements.find(line_word) != replacements.end()) {
      out_word = "_RARE_";
    }
    std::cout << out_word << " " << line_stream.rdbuf() << std::endl;
  }
  return 0;
}

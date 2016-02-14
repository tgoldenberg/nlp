#ifndef EX1_LIB_H

#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <fstream>

typedef std::pair<std::string, std::string> EmissionPair;

struct ModelLine {
  int count;
  std::string type;
  std::string tag;   
  std::string word;
};

class ModelReader {
 public:
  ModelReader(std::ifstream& input_file);
  bool YieldLine(ModelLine* model_line);
 private:
  std::ifstream& input_file_;
};

std::unordered_map <std::string, int> get_word_counts(ModelReader& model_reader);

class SimpleTagger {
 public:
  SimpleTagger();

  void BuildModel(ModelReader& model_reader);
  std::string TagWord(const std::string& word, bool fallback_to_rare = true);
 private:
  std::map<std::string, std::map<std::string, int>> emission_counts_;
  std::map<std::string, int> tag_counts_;
};
 
#endif

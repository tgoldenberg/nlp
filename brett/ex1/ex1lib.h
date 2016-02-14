#ifndef EX1_LIB_H

#include <fstream>
#include <map>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

typedef std::pair<std::string, std::string> EmissionPair;

struct TaggedWord {
  std::string word;
  std::string tag;
};

struct DataLine {
  std::vector<TaggedWord> tagged_words;
};

class DataReader {
 public:
  DataReader(std::ifstream& input_file);
  bool YieldLine(DataLine* model_line);
 private:
  std::ifstream& input_file_;   
};

struct CountLine {
  int count;
  std::string type;
  std::string tag;   
  std::string word;
};

class CountReader {
 public:
  CountReader(std::ifstream& input_file);
  bool YieldLine(CountLine* model_line);
 private:
  std::ifstream& input_file_;
};

std::unordered_map <std::string, int> get_word_counts(CountReader& model_reader);

class SimpleTagger {
 public:
  SimpleTagger();

  void BuildModel(CountReader& count_reader);
  std::string TagWord(const std::string& word, bool fallback_to_rare = true);
 private:
  std::map<std::string, std::map<std::string, int>> emission_counts_;
  std::map<std::string, int> tag_counts_;
};
 
#endif

#ifndef EX1_LIB_H

#include <map>
#include <set>
#include <string>
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
  DataReader(std::istream& input_file);
  bool YieldLine(DataLine* model_line);
 private:
  std::istream& input_file_;
};

struct CountLine {
  int count;
  std::string type;
  std::string word0;
  std::string word1;
  std::string word2;
  std::string word3;
};

class CountReader {
 public:
  CountReader(std::istream& input_file);
  bool YieldLine(CountLine* model_line);
 private:
  std::istream& input_file_;
};

std::map<std::string, int> get_word_counts(CountReader& model_reader);

class SimpleTagger {
 public:
  SimpleTagger();

  virtual void BuildModel(CountReader& count_reader);
  std::string TagWord(const std::string& word, bool fallback_to_rare = true);
 private:
  std::map<std::string, std::map<std::string, int>> emission_counts_;
  std::map<std::string, int> tag_counts_;
};

class ViterbiTagger {
 public:
  void BuildModel(CountReader& count_reader);
  float GetTrigramProbability(const std::string& t0,
                              const std::string& t1,
                              const std::string& t2) const;
  float GetEmissionProbability(const std::string& tag,
                               const std::string& word) const;
  const std::set<std::string>& GetTags() const;
  const std::set<std::string>& GetWordsInModel() const;
  std::vector<TaggedWord> TagSentence(const std::vector<std::string>& sentence);
 private:
  std::map<std::string, std::map<std::string, std::map<std::string, int>>> tag_trigrams_;
  std::map<std::string, std::map<std::string, int>> tag_bigrams_;

  std::map<std::string, std::map<std::string, int>> emission_counts_;
  std::map<std::string, int> tag_counts_;
  std::set<std::string> tags_;
  std::set<std::string> words_in_model_;
};

struct ViterbiTriple {
  int spot;
  const std::string& u;
  const std::string& v;
  bool operator<(const ViterbiTriple& rhs) const;
};

class ViterbiSolver {
 public:
  ViterbiSolver(
    const std::vector<std::string>& sentence,
    const ViterbiTagger& viterbi_tagger);
  float Pi(const std::string& u, const std::string& v, int spot);
  std::string SampleSentence(int i) const;
  std::vector<TaggedWord> TagSentence();
 private:
  const std::vector<std::string>& sentence_;
  const ViterbiTagger& viterbi_tagger_;
  std::map<ViterbiTriple, int> pi_;
  std::map<ViterbiTriple, std::string> back_pointers_;
};

#endif

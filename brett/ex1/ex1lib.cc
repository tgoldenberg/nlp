#include "ex1lib.h"
#include "ex1maputil.h"

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using std::ifstream;
using std::map;
using std::set;
using std::string;
using std::stringstream;
using std::vector;

DataReader::DataReader(ifstream& input_file) : input_file_(input_file) {}

bool DataReader::YieldLine(DataLine* data_line) {
  string line;
  // go to the first non empty line
  bool found_non_empty_line = false;
  while (std::getline(input_file_, line)) { 
    if (line.empty()) {
      found_non_empty_line = true;
      break;
    }   
  }

  // read consecutive lines
  if (!found_non_empty_line) {
    return false;
  }

  while (std::getline(input_file_, line)) {
    if (line.empty()) {
      break;
    }
    stringstream line_stream(line);
    TaggedWord tagged_word;
    line_stream >> tagged_word.word;
    line_stream >> tagged_word.tag;
    data_line->tagged_words.push_back(tagged_word);
  }

  return (data_line->tagged_words.size() > 0);
}

CountReader::CountReader(ifstream& input_file) : input_file_(input_file) {}
 
bool CountReader::YieldLine(CountLine* count_line) {
  string line;
  while (std::getline(input_file_, line)) {
    if (!line.empty()) {
      stringstream line_stream(line);
      line_stream >> count_line->count;
      line_stream >> count_line->type;
      line_stream >> count_line->word0;
      line_stream >> count_line->word1;
      line_stream >> count_line->word2;
      line_stream >> count_line->word3;

      return true;
    }
  }

  return false;
}

map<string, int> get_word_counts(CountReader& count_reader) {
  map<std::string, int> word_counts;
  CountLine count_line;
  while (count_reader.YieldLine(&count_line)) {
    if (count_line.type == "WORDTAG") {
      add_1_key_value_to_map(count_line.word1, count_line.count, word_counts);
    }
  }
  return word_counts;
}
SimpleTagger::SimpleTagger() {}

void SimpleTagger::BuildModel(CountReader& count_reader) {
  CountLine count_line;
  while (count_reader.YieldLine(&count_line)) {
    if (count_line.type == "WORDTAG") {
      add_1_key_value_to_map(count_line.word0, count_line.count, tag_counts_);
      add_2_key_value_to_map(count_line.word0, count_line.word1, count_line.count, emission_counts_);
    }
  }
}

string SimpleTagger::TagWord(const string& word, bool fallback_to_rare) {
  // if the word is not in the model, we will use _RARE_ instead...
  // if that is not in the model, we just say zero.
  bool found = false;
  float max_tag_percentage = 0.0f;
  std::string tag = "O";

  // for each possible tag, get the probability that this tag is this word.
  // choose the tag that has maximum probability of generating this word.
  for (auto& tag_emissions : emission_counts_) {
    int word_count = 0;
    if (get_1_key_value_from_map(word, tag_emissions.second, &word_count)) {
      found = true;
      float tag_percentage = (float)word_count/(float)tag_counts_[tag_emissions.first];
      if (tag_percentage >= max_tag_percentage) {
        tag = tag_emissions.first;
        max_tag_percentage = tag_percentage;
      }
    }
  }
  if (!found && fallback_to_rare) {
    return TagWord("_RARE_", false);
  }
  return tag.c_str();
}

void ViterbiTagger::BuildModel(CountReader& count_reader) {
  CountLine count_line;
  tags_.insert("*");
  tags_.insert("STOP");
  while (count_reader.YieldLine(&count_line)) {
    if (count_line.type == "WORDTAG") {
      add_1_key_value_to_map(count_line.word0, count_line.count, tag_counts_);
      add_2_key_value_to_map(count_line.word0, count_line.word1, count_line.count, emission_counts_);
      words_in_model_.insert(count_line.word1);
      tags_.insert(count_line.word0);
    } else if (count_line.type == "2-GRAM") {
      add_2_key_value_to_map(count_line.word0, count_line.word1, count_line.count, tag_bigrams_);
    } else if (count_line.type == "3-GRAM") {
      add_3_key_value_to_map(count_line.word0, count_line.word1, count_line.word2, count_line.count, tag_trigrams_);
    }
  }
}

float ViterbiTagger::GetTrigramProbability(
    const string& t0,
    const string& t1,
    const string& t2) const {
  int count_trigram = 0;
  int count_bigram = 0;
  if (get_3_key_value_from_map(t0, t1, t2, tag_trigrams_, &count_trigram) &&
      get_2_key_value_from_map(t0, t1, tag_bigrams_, &count_bigram)) {
    return (float)count_trigram/(float)count_bigram;  
  }
  return 0.0f;
}

float ViterbiTagger::GetEmissionProbability(
    const string& tag, const string& word) const {
  // For each tag pick 
  int tag_count = 0;
  int word_tag_count = 0;
  if (get_1_key_value_from_map(tag, tag_counts_, &tag_count) &&
      get_2_key_value_from_map(tag, word, emission_counts_, &word_tag_count)) {
    return (float)word_tag_count/(float)tag_count;
  }
  return 0.0f;
}

const set<string>& ViterbiTagger::GetTags() const {
  return tags_;
}

const set<string>& ViterbiTagger::GetWordsInModel() const {
  return words_in_model_;
}

vector<TaggedWord> ViterbiTagger::TagSentence(const vector<string>& sentence) {
  ViterbiSolver solver(sentence, *this);
  return solver.TagSentence();
}


bool ViterbiTriple::operator<(const ViterbiTriple& rhs) const {
  if (spot != rhs.spot) {
    return spot < rhs.spot;
  }
  if (u != rhs.u) {
    return u < rhs.u;
  }
  return v < rhs.v;
}

ViterbiSolver::ViterbiSolver(
    const vector<string>& sentence,
    const ViterbiTagger& viterbi_tagger) : sentence_(sentence),
                                           viterbi_tagger_(viterbi_tagger) {}

string ViterbiSolver::SampleSentence(int i) const {
  if (i < 0) {
    return "*";
  } else if (i >= sentence_.size()) {
    return "STOP";
  } else {
    // Add rare logic sampling here.
    if (viterbi_tagger_.GetWordsInModel().find(sentence_[i]) != 
        viterbi_tagger_.GetWordsInMOdel().end()) {
      return sentence_[i];
    } else {
      return "_RARE_";
    }
  }
}

vector<TaggedWord> ViterbiSolver::TagSentence() {
  for (const string& tag : viterbi_tagger_.GetTags()) {
    Pi(tag, "STOP", sentence_.size() + 1);
  }
  return {};
}

float ViterbiSolver::Pi(const string& u, const string& v, int spot) {
  ViterbiTriple vt = {spot, u, v};
  if (spot == 0) {
    if (u == "*" && v == "*") {
      return 1.0f;
    } else {
      return 0.0f;
    }
  }

  if (pi_.find(vt) != pi_.end()) {
    return pi_[vt];   
  }

  float max_prob = 0.0f;
  string max_tag = "";
  for (const string& tag_w : viterbi_tagger_.GetTags()) {
    float w_prob = Pi(tag_w, u, spot - 1) *
                   viterbi_tagger_.GetTrigramProbability(tag_w, u, v) *
                   viterbi_tagger_.GetEmissionProbability(
                    tag_w, SampleSentence(spot));
    if (w_prob > max_prob) {
      max_prob = w_prob;
      max_tag = tag_w;
    }
  }

  pi_[vt] = max_prob;
  back_pointers_[vt] = max_tag;
 
  return max_prob; 
}

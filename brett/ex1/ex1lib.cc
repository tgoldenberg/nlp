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

ModelReader::ModelReader(ifstream& input_file) : input_file_(input_file) {}
 
bool ModelReader::YieldLine(ModelLine* model_line) {
  string line;
  while (std::getline(input_file_, line)) {
    if (!line.empty()) {
      stringstream line_stream(line);
      line_stream >> model_line->count;
      line_stream >> model_line->type;
      line_stream >> model_line->tag;
      line_stream >> model_line->word;
      return true;
    }
  }

  return false;
}

unordered_map<string, int> get_word_counts(ModelReader& model_reader) {
  unordered_map<std::string, int> word_counts;
  ModelLine model_line;
  while (model_reader.YieldLine(&model_line)) {
    if (model_line.type == "WORDTAG") {
      int existing_count = 0;
      auto word_count_it = word_counts.find(model_line.word);
      if (word_count_it != word_counts.end()) {
        existing_count = word_count_it->second;
      }
      word_counts[model_line.word] = existing_count + model_line.count;
    }
  }
  return word_counts;
}

SimpleTagger::SimpleTagger() {}

void SimpleTagger::BuildModel(ModelReader& model_reader) {
  ModelLine model_line;
  while (model_reader.YieldLine(&model_line)) {
    if (model_line.type == "WORDTAG") {
      auto count_it = emission_counts_.find(model_line.tag);
      if (count_it == emission_counts_.end()) {
          emission_counts_[model_line.tag] = std::map<string, int>();          
      }
      emission_counts_[model_line.tag][model_line.word] = model_line.count;
      int current_tag_count = 0;
      const auto tag_count_it = tag_counts_.find(model_line.tag);
      if (tag_count_it != tag_counts_.end()) {
        current_tag_count = tag_count_it->second;
      }
      tag_counts_[model_line.tag] = current_tag_count + model_line.count;
    }
  }
}

string SimpleTagger::TagWord(const string& word, bool fallback_to_rare) {
  // if the word is not in the model, we will use _RARE_ instead...
  // if that is not in the model, we just say zero.
  bool found = false;
  float max_tag_percentage = 0.0f;
  std::string tag = "O";
  for (auto& tag_emissions : emission_counts_) {
    const auto word_it = tag_emissions.second.find(word);
    if (word_it != tag_emissions.second.end()) {
      found = true;
      float tag_percentage = (float)word_it->second/(float)tag_counts_[tag_emissions.first];
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

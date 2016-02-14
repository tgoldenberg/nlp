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
      line_stream >> count_line->tag;
      line_stream >> count_line->word;
      return true;
    }
  }

  return false;
}

unordered_map<string, int> get_word_counts(CountReader& count_reader) {
  unordered_map<std::string, int> word_counts;
  CountLine count_line;
  while (count_reader.YieldLine(&count_line)) {
    if (count_line.type == "WORDTAG") {
      int existing_count = 0;
      auto word_count_it = word_counts.find(count_line.word);
      if (word_count_it != word_counts.end()) {
        existing_count = word_count_it->second;
      }
      word_counts[count_line.word] = existing_count + count_line.count;
    }
  }
  return word_counts;
}

SimpleTagger::SimpleTagger() {}

void SimpleTagger::BuildModel(CountReader& count_reader) {
  CountLine count_line;
  while (count_reader.YieldLine(&count_line)) {
    if (count_line.type == "WORDTAG") {
      auto count_it = emission_counts_.find(count_line.tag);
      if (count_it == emission_counts_.end()) {
          emission_counts_[count_line.tag] = std::map<string, int>();          
      }
      emission_counts_[count_line.tag][count_line.word] = count_line.count;
      int current_tag_count = 0;
      const auto tag_count_it = tag_counts_.find(count_line.tag);
      if (tag_count_it != tag_counts_.end()) {
        current_tag_count = tag_count_it->second;
      }
      tag_counts_[count_line.tag] = current_tag_count + count_line.count;
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

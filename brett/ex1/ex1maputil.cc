#include "ex1maputil.h"

using std::map;
using std::string;

void add_1_key_value_to_map(const string& key0,
                            int count,
                            map<string, int>& map_to_add_to) {
  auto count_it = map_to_add_to.find(key0);
  int current_value = 0;
  if (count_it != map_to_add_to.end()) {
    current_value = count_it->second;
  }
  map_to_add_to[key0] = current_value + count;
}

void add_2_key_value_to_map(const string& key0,
                            const string& key1,
                            int count,
                            map<string, map<string, int>>& map_to_add_to) {
  auto count_it = map_to_add_to.find(key0);
  if (count_it == map_to_add_to.end()) {
    map_to_add_to[key0] = map<string, int>();          
  }
  add_1_key_value_to_map(key1, count, map_to_add_to[key0]);
}

void add_3_key_value_to_map(const string& key0,
                            const string& key1,
                            const string& key2,
                            int count,
                            map<string, map<string, map<string, int>>>& map_to_add_to) {
  auto count_it = map_to_add_to.find(key0);
  if (count_it == map_to_add_to.end()) {
    map_to_add_to[key0] = map<string, map<string, int>>();          
  }
  add_2_key_value_to_map(key1, key2, count, map_to_add_to[key0]);
}

bool get_1_key_value_from_map(const std::string& key0,
                              const std::map<std::string, int>& map_to_get_from,
                              int* value) {
  auto count_it = map_to_get_from.find(key0);
  if (count_it != map_to_get_from.end()) {
      *value = count_it->second;
      return true;
  }
  return false;
}

bool get_2_key_value_from_map(const std::string& key0,
                              const std::string& key1,
                              const std::map<std::string, std::map<std::string, int>>& map_to_get_from,
                              int* value) {
  auto count_it = map_to_get_from.find(key0);
  if (count_it != map_to_get_from.end()) {
      return get_1_key_value_from_map(key1, count_it->second, value);
  }
  return false;
}

bool get_3_key_value_from_map(const std::string& key0,
                              const std::string& key1,
                              const std::string& key2,
                              const std::map<std::string, std::map<std::string, std::map<std::string, int>>>& map_to_get_from,
                              int* value) {
  auto count_it = map_to_get_from.find(key0);
  if (count_it != map_to_get_from.end()) {
      return get_2_key_value_from_map(key1, key2, count_it->second, value);
  }
  return false;
}

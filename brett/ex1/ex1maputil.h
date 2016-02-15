#ifndef EX1_MAPUTIL_H
#define EX1_MAPUTIL_H

#include <map>
#include <string>

void add_1_key_value_to_map(const std::string& key0,
                            int count,
                            std::map<std::string, int>& map_to_add_to);
void add_2_key_value_to_map(const std::string& key0,
                            const std::string& key1,
                            int count,
                            std::map<std::string, std::map<std::string, int>>& map_to_add_to);
void add_3_key_value_to_map(const std::string& key0,
                            const std::string& key1,
                            const std::string& key2,
                            int count,
                            std::map<std::string, std::map<std::string, std::map<std::string, int>>>& map_to_add_to);

bool get_1_key_value_from_map(const std::string& key0,
                              const std::map<std::string, int>& map_to_get_from,
                              int* value);
bool get_2_key_value_from_map(const std::string& key0,
                              const std::string& key1,
                              const std::map<std::string, std::map<std::string, int>>& map_to_get_from,
                              int* value);
bool get_3_key_value_from_map(const std::string& key0,
                              const std::string& key1,
                              const std::string& key2,
                              const std::map<std::string, std::map<std::string, std::map<std::string, int>>>& map_to_get_from,
                              int* value);
#endif

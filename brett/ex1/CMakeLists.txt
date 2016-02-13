cmake_minimum_required (VERSION 2.8.11)
project (ex1)

add_library (ex1lib ex1lib.cc)
add_executable (replace_low_frequency_words replace_low_frequency_words.cc)
target_link_libraries(replace_low_frequency_words ex1lib)

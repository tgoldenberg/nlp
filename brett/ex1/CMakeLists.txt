cmake_minimum_required (VERSION 2.8.11)

project (ex1)

# figure out how to do to something more portable here regarding c++11
SET(CMAKE_CXX_FLAGS "-std=c++11")

add_library (ex1lib ex1lib.cc)

add_executable (get_low_frequency_words get_low_frequency_words.cc)
target_link_libraries(get_low_frequency_words ex1lib)

add_executable (replace_low_frequency_words replace_low_frequency_words.cc)
target_link_libraries(replace_low_frequency_words ex1lib)

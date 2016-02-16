cmake_minimum_required (VERSION 2.8.11)

ENABLE_LANGUAGE(C)

include(ExternalProject)
find_package (Threads)

project (ex1)

# Download and install GoogleTest
ExternalProject_Add(
    gtest
    URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND ""
)

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set gtest properties
ExternalProject_Get_Property(gtest source_dir binary_dir)
set_target_properties(libgtest PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
)

include_directories("${source_dir}/include")

# Download and install GoogleMock
ExternalProject_Add(
    gmock
    URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
    # Disable install step
    INSTALL_COMMAND ""
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gmock)

# Set gmock properties
ExternalProject_Get_Property(gmock source_dir binary_dir)
set_target_properties(libgmock PROPERTIES
    "IMPORTED_LOCATION" "${binary_dir}/libgmock.a"
    "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
#    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
)
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

SET(CMAKE_CXX_FLAGS "-std=c++11")

add_library (ex1lib ex1maputil.cc ex1lib.cc)

add_executable (get_low_frequency_words get_low_frequency_words.cc)
target_link_libraries(get_low_frequency_words ex1lib)

add_executable (replace_low_frequency_words replace_low_frequency_words.cc)
target_link_libraries(replace_low_frequency_words ex1lib)

add_executable (simple_tagger simple_tagger.cc)
target_link_libraries(simple_tagger ex1lib)

add_executable (viterbi_tagger viterbi_tagger.cc)
target_link_libraries(viterbi_tagger ex1lib)

add_executable(viterbi_tagger_test viterbi_tagger_test.cc)
target_link_libraries(viterbi_tagger_test
    ex1lib
    ${CMAKE_THREAD_LIBS_INIT}
    libgtest
    libgmock
    pthread)

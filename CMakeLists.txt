cmake_minimum_required(VERSION 3.28.3)
project(PPAA)

if (${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Minnow must be built outside its source directory, e.g. `cmake -B build`.")
endif()

file (GLOB_RECURSE ALL_SRC_FILES *.hpp *.cpp)
add_custom_target (format "clang-format" -i ${ALL_SRC_FILES} COMMENT "Formatting source code...")

add_subdirectory(membw)
add_subdirectory(PA)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
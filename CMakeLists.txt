cmake_minimum_required(VERSION 3.28.3)
project(PPAA)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(NoInSourceBuilds)

# add_subdirectory(membw)
add_subdirectory(PA)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
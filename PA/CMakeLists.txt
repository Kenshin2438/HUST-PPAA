project(PA LANGUAGES CXX)

include(OneTBB)
# include(GoogleBenchmark)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -pthread")

add_subdirectory(PA1-1)
add_subdirectory(PA1-2)
add_subdirectory(PA2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra -Wfloat-equal -Wshadow -Wconversion -Wno-sign-conversion -Wno-unused-const-variable")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

find_package(TBB REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} PUBLIC TBB::tbb)
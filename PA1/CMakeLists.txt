set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE})
    target_link_libraries(${EXECUTABLE_NAME} m pthread)
endforeach()
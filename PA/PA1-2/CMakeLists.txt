project(PA1-2 LANGUAGES CXX)

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    add_executable(${PROJECT_NAME}-${EXECUTABLE_NAME} ${SOURCE})
endforeach()
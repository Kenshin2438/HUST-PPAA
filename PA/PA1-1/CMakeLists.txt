project(PA1-1 LANGUAGES CXX)

# add_compile_definitions(EULER_SIEVE)
# if (WIN32) # set stack-size = 256MB ()
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,268435456")
# endif()

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
foreach(SOURCE ${SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    add_executable(${PROJECT_NAME}-${EXECUTABLE_NAME} ${SOURCE})
endforeach()
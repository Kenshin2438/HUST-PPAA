if(${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR
    "Project must be built outside its source directory, e.g:\n"
    "  cmake -B build\n"
    "To remove files you accidentally created execute:\n"
    "  rm -rf CMakeFiles CMakeCache.txt\n"
  )
endif()
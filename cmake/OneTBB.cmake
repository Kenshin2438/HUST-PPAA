find_package(TBB QUIET)
if(NOT TBB_FOUND)
  message(STATUS "TBB is not found. Downloading.")
  include(FetchContent)
  FetchContent_Declare(TBB
    GIT_REPOSITORY https://github.com/oneapi-src/oneTBB.git
    GIT_TAG v2022.0.0
  )
  option(TBB_TEST "Enable testing" OFF)
  FetchContent_MakeAvailable(TBB)
endif()
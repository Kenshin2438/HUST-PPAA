project(membw LANGUAGES ASM C)
message(WARNING
  "Project membw should be run on https://course.cunok.cn:52443"
  "\nIgnore if on the correct environment."
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
# Warning: missing .note.GNU-stack section implies executable stack
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack")

add_executable(mbw x86-asm.S mbw.c util.c)
target_link_libraries(mbw m pthread)

add_executable(pmbw pmbw.c util.c)
target_link_libraries(pmbw m pthread)

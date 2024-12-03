project(membw LANGUAGES ASM C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")
# Warning: missing .note.GNU-stack section implies executable stack
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -z noexecstack")

add_executable(membw x86-asm.S main.c util.c)
target_link_libraries(membw m pthread)

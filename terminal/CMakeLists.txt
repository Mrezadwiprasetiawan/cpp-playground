include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB_RECURSE SOURCES src/*.cxx)

add_executable(terminal_ANSI-playground ${SOURCES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

set(SRC "src/")
add_executable(move ${SRC}move.cxx)
add_test(NAME move COMMAND move)

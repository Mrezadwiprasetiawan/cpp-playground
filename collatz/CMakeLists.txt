INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_compile_options(-Wno-shift-op-parentheses)
add_executable(collatz src/collatz.cpp)

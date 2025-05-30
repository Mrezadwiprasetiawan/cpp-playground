include_directories(include)
set(SRC "src")

add_executable(endianness ${SRC}/endianness.cxx)
add_test(NAME TestEndianness COMMAND endianness)

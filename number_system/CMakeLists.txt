#Include direktori header
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

# File sumber
set(SRC_SOURCES "src/")

# Untuk Testing
add_executable(big_int_test ${SRC_SOURCES}/big_int.cxx)
#add_test(NAME "Test big_int" COMMAND big_int_test)

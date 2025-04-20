#Include direktori header
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

# File sumber
set(SRC_SOURCES "src/")

# Buat executable
add_executable(prime ${SRC_SOURCES}prime.cxx)
add_executable(fibonacci ${SRC_SOURCES}fibonacci.cxx)

#library
add_library(big_int SHARED ${SRC_SOURCES}/big_int.cxx)
#add_library(big_int STATIC ${SRC_SOURCES}/big_int.cxx)

# Untuk Testing
add_executable(big_int_test ${SRC_SOURCES}/big_int.cxx test/big_int.cxx)

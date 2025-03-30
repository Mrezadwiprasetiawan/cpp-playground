# Include direktori header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# File sumber
set(SRC_SOURCES "src/")

# Buat executable
add_executable(op_overload_test ${SRC_SOURCES}op_overload.cxx) 

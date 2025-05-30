# Include direktori header
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

# File sumber
set(SRC_SOURCES "src/")

# Buat executable
add_executable(args_handle ${SRC_SOURCES}args_handle.cxx)
add_test(NAME TestArgsHandle COMMAND args_handle)

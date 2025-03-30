# Include direktori header
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# File sumber
set(SRC_SOURCES "src/")

# Buat executable
add_executable(assert ${SRC_SOURCES}assert.cxx)

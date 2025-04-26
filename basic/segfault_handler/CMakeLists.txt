include_directories(${CMAKE_CURRENT_SOURCE_DIR}include)

set(SRC "src/")
add_executable(segfaulthandler ${SRC}segfault_handler.cxx)

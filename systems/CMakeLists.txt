include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library(systems INTERFACE)
target_include_directories(systems INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(linear INTERFACE)
target_include_directories(linear INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

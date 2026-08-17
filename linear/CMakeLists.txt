include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(linear INTERFACE)
target_include_directories(linear INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(mat_test src/mat_test.cpp)
add_executable(matNN src/NN.cpp)

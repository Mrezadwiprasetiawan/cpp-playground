include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR})

set(SRC "src")



add_executable(prime-art ${SRC}/prime-art.cxx)
find_package(PNG REQUIRED)
target_link_libraries(prime-art PRIVATE PNG::PNG discrete)

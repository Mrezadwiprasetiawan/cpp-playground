include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR})

set(SRC "src")

find_package(PNG REQUIRED)

add_executable(prime-art ${SRC}/prime-art.cxx)
target_link_libraries(prime-art PNG::PNG)

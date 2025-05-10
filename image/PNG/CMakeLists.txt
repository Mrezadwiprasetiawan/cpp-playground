include_directories(include)

#untuk kelas prime jadi include header dari subproject number_system
include_directories(${CMAKE_SOURCE_DIR}/discrete/include)

set(SRC "src")

find_package(PNG REQUIRED)

add_executable(prime-art ${SRC}/prime-art.cxx)
target_link_libraries(prime-art PNG::PNG)

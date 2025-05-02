include_directories(include)

#untuk kelas prime jadi include header dari subproject number_system
include_directories(${CMAKE_SOURCE_DIR}/number_system/include)

set(SRC "src")

find_package(PNG REQUIRED)

add_executable(pngw ${SRC}/pngwriter.cxx)
target_link_libraries(pngw PNG::PNG)

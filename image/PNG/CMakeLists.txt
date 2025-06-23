include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR})

set(SRC "src")



add_executable(prime-art ${SRC}/prime-art.cxx)
if(CMAKE_SYSTEM_NAME STREQUAL "Android")
  target_link_libraries(prime-art ${ANDROID_NDK}/sources/third_party/libpng)
else()
  find_package(PNG REQUIRED)
  target_link_libraries(prime-art PNG::PNG)
endif()

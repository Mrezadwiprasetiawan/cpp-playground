find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
  message(STATUS "Found OpenCV include: ${OpenCV_INCLUDE_DIRS}")
  message(STATUS "Found OpenCV libs: ${OpenCV_LIBS}")
else()
  message(FATAL_ERROR "OpenCV not found.")
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OpenCV_INCLUDE_DIRS}
)

set(QT_FEATURE_dbus OFF)

file(GLOB_RECURSE SOURCES src/*.cxx)

add_executable(testtermdisp src/test_display.cxx)
add_executable(testrendervid src/test-render-video.cxx)
add_executable(testcolls src/collision.cxx)

target_link_libraries(testrendervid PRIVATE ${OpenCV_LIBS})
target_link_libraries(testtermdisp PRIVATE ${OpenCV_LIBS})
target_include_directories(testrendervid PRIVATE
  ${OpenCV_INCLUDE_DIRS}
  ${OpenCV_INCLUDE_DIRS}/opencv4
)

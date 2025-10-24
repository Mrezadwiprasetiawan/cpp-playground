# Pastikan OpenCV sudah ditemukan dari root atau cari ulang
find_package(OpenCV REQUIRED)

if(OpenCV_FOUND)
  message(STATUS "✅ Found OpenCV include: ${OpenCV_INCLUDE_DIRS}")
  message(STATUS "✅ Found OpenCV libs: ${OpenCV_LIBS}")
else()
  message(FATAL_ERROR "❌ OpenCV not found.")
endif()

# Pastikan path header OpenCV diterapkan di target
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${OpenCV_INCLUDE_DIRS}
)

file(GLOB_RECURSE SOURCES src/*.cxx)

add_executable(testtermdisp src/test_display.cxx)
add_executable(testrendervid src/test-render-video.cxx)

# Link ke OpenCV
target_link_libraries(testrendervid PRIVATE ${OpenCV_LIBS})

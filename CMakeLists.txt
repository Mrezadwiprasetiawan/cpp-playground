cmake_minimum_required(VERSION 3.31.2)
project("myCpp-playground" VERSION 0.1 LANGUAGES C CXX)

enable_testing()
# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Set C++ standard (opsional)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}-O3 -march=native -funroll-loops -ffast-math")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
add_subdirectory(win32)
endif()

# Add subprojects
add_subdirectory(basic)
add_subdirectory(number_system)
add_subdirectory(3D)
add_subdirectory(image)
add_subdirectory(discrete)
add_subdirectory(NN)

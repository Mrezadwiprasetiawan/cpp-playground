include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ../../linear/include)
find_package(Vulkan REQUIRED)
link_libraries(Vulkan::Vulkan)

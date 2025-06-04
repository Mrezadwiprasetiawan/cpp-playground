add_library(testModule INTERFACE)
target_sources(testModule INTERFACE module.ixx)

add_executable(testModuleMain main.cpp)
target_link_libraries(testModuleMain PRIVATE testModule) 
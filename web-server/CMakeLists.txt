include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)
add_executable(web_test src/main.cxx)
add_executable(server src/server.cxx)
add_executable(client src/client.cxx)
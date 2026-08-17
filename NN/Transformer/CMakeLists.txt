cmake_minimum_required(VERSION 3.20)
project(Transformer CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Utility/include ../../linear/include)

add_executable(transformer src/main.cpp)

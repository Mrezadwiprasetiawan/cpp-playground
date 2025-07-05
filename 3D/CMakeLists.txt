include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/linear/include)

#library utama
#add_librar(3d INTERFACE)
#target_include_directories(3d INTERFACE include/)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)


#untuk testing
add_executable(test_3d ${SRC}/test_3d.cxx)
add_test(NAME Test3D COMMAND test_3d)

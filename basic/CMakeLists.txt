add_subdirectory(args_handle)
add_subdirectory(assertion)
add_subdirectory(operator_overloading)
if(NOT WIN32)
  add_subdirectory(segfault_handler)
endif()
add_subdirectory(move)
add_subdirectory(endianness)

# temporary exclude module
# add_subdirectory(module)
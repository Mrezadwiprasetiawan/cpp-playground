add_executable(win32app app.cxx)

# Supaya tidak muncul console (khusus untuk Win32 GUI)
set_target_properties(win32app PROPERTIES
  WIN32_EXECUTABLE YES
)

target_link_libraries(winn32app PRIVATE user32 gdi32 kernel32)
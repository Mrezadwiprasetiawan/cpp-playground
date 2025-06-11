add_executable(win32-create_window app.cxx)

# Supaya tidak muncul console (khusus untuk Win32 GUI)
set_target_properties(win32-create_window PROPERTIES WIN32_EXECUTABLE YES)
target_link_libraries(win32-create_window PRIVATE user32 gdi32 kernel32)
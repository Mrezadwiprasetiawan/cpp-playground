add_executable(win32-add_menus app.cxx)

# Supaya tidak muncul console (khusus untuk Win32 GUI)
set_target_properties(win32-add_menus PROPERTIES WIN32_EXECUTABLE YES)
target_link_libraries(win32-add_menus PRIVATE user32 gdi32 kernel32)
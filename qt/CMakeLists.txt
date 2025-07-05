set(CMAKE_AUTOMOC ON)  # dibutuhkan karena Q_OBJECT
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(app main.cxx Window.hxx)

target_link_libraries(app
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

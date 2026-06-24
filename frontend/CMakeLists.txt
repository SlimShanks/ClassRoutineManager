cmake_minimum_required(VERSION 3.16)

project(ClassRoutineFrontend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

# -------------------------
# Frontend Executable
# -------------------------
add_executable(ClassRoutineManagerUI
main.cpp
    src/MainWindow.cpp
    src/dialogs/AddTeacherDialog.cpp
    src/dialogs/TeacherListDialog.cpp
    src/views/RoutineView.cpp
    src/views/RoutineSetupWidget.cpp
    src/views/RoutineGridWidget.cpp
    src/views/TeacherBlockWidget.cpp
    src/views/GridCanvas.h

)

target_include_directories(ClassRoutineManagerUI PRIVATE
    src
)

target_link_libraries(ClassRoutineManagerUI PRIVATE
    Qt6::Widgets
    Qt6::Sql
    backend_lib   # 🔥 IMPORTANT: connects frontend to backend
)
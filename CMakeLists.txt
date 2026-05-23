cmake_minimum_required(VERSION 3.16)
project(ClassRoutineManager VERSION 1.0 LANGUAGES CXX)

if(APPLE)
    set(CMAKE_PREFIX_PATH "/opt/homebrew/opt/qt")
elseif(WIN32)
    set(CMAKE_PREFIX_PATH "C:/Qt/6.8.0/mingw_64")
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/dbmanager.cpp
    src/authmanager.cpp
    src/routinemanager.cpp
)

qt_add_executable(ClassRoutineManager
    ${SOURCES}
)

target_link_libraries(ClassRoutineManager PRIVATE
    Qt6::Widgets
    Qt6::Sql
)

target_include_directories(ClassRoutineManager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

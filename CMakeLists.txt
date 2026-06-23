cmake_minimum_required(VERSION 3.16)
project(ClassRoutineManager VERSION 1.0 LANGUAGES CXX)

if(NOT DEFINED CMAKE_PREFIX_PATH)
    if(APPLE)
        set(CMAKE_PREFIX_PATH "/opt/homebrew/Cellar/qtbase/6.11.1")
    elseif(WIN32)
        set(CMAKE_PREFIX_PATH "C:/Qt/6.8.0/mingw_64")
    elseif(UNIX AND NOT APPLE)
        set(CMAKE_PREFIX_PATH "/usr/lib/x86_64-linux-gnu/qt6")
    endif()

endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)


add_subdirectory(backend)
add_subdirectory(frontend)


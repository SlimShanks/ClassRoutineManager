# as from syntax we know this is the cmake VERSION we rely on
cmake_minimum_required(VERSION 3.16)

# Just the Project name and the project VERSION
project(ClassRoutineBakend VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql)

# all the .cpp files that need to be compiled into the program
set(SOURCES
    src/main.cpp
    src/appcontext.cpp
    src/database/SQLiteDB.cpp
    src/database/MysqlDB.cpp
    src/database/SyncManager.cpp
    src/utils/Logger.cpp
)

add_executable(ClassRoutineManager
    ${SOURCES}
)

target_include_directories(ClassRoutineManager PRIVATE
    src
)

target_link_libraries(ClassRoutineManager PRIVATE
    Qt6::Core
    Qt6::Sql
)

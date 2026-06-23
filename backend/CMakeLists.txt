cmake_minimum_required(VERSION 3.16)

project(ClassRoutineBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql)

add_library(backend_lib STATIC
    src/appcontext.cpp
    src/database/SQLiteDB.cpp
    src/database/MysqlDB.cpp
    src/database/SyncManager.cpp
    src/services/AuthService.cpp
    src/services/TeacherService.cpp
    src/repositories/UserRepo.cpp
    src/utils/Logger.cpp
)

target_include_directories(backend_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(backend_lib PUBLIC
    Qt6::Core
    Qt6::Sql
)
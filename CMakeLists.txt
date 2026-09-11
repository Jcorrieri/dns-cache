cmake_minimum_required(VERSION 3.16)

project(MyApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(app
    src/main.cpp
    src/cache.cpp
    src/database.cpp
    external/sqlite/sqlite3.c
)

target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/external/sqlite
)

target_include_directories(app PRIVATE
    ${CMAKE_SOURCE_DIR}/includes
)

target_compile_options(app PRIVATE
    -Wall
    -Wextra
)

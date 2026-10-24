cmake_minimum_required(VERSION 3.16)

project(MyApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(ProjectBase INTERFACE)

target_compile_options(ProjectBase
    INTERFACE
        -Wall
        -Wextra
)

target_include_directories(ProjectBase
    INTERFACE
        common/includes
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(ProjectBase INTERFACE 
    Threads::Threads
)

add_executable(Server
    server/src/main.cpp
    server/src/cache.cpp
    server/src/ip.cpp
    server/src/producer_consumer_queue.cpp
    server/src/database.cpp
    server/src/record_repository.cpp
    server/src/parse_utils.cpp
    external/sqlite/sqlite3.c
)

target_include_directories(Server PRIVATE
    ${CMAKE_SOURCE_DIR}/external/sqlite
)

target_include_directories(Server PRIVATE
    server/includes
)

target_link_libraries(Server PRIVATE
    ProjectBase
)

add_executable(Client
    client/src/main.cpp
)

target_link_libraries(Client PRIVATE
    ProjectBase
)

cmake_minimum_required(VERSION 3.16)

project(DnsCache LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(common_options INTERFACE)

target_compile_options(common_options
    INTERFACE
        -Wall
        -Wextra
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(common_options INTERFACE 
    Threads::Threads
)

add_library(common_impl STATIC
    common/src/common.cpp
)

target_include_directories(common_impl PUBLIC
    common/includes
)

target_link_libraries(common_impl PUBLIC common_options)

add_executable(server
    server/src/main.cpp
    server/src/cache.cpp
    server/src/ip.cpp
    server/src/producer_consumer_queue.cpp
    server/src/database.cpp
    server/src/record_repository.cpp
    server/src/parse_utils.cpp
    server/src/worker.cpp
    external/sqlite/sqlite3.c
)

target_include_directories(server PRIVATE
    ${CMAKE_SOURCE_DIR}/external/sqlite
)

target_include_directories(server PRIVATE
    server/includes
)

target_link_libraries(server PRIVATE
    common_impl
)

add_executable(client
    client/src/main.cpp
)

target_link_libraries(client PRIVATE
    common_impl
)

cmake_minimum_required(VERSION 3.16)

project(DnsCache LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)

set(DNS_CACHE_VARIANT "naive" CACHE STRING "Cache implementation variant")
set_property(
    CACHE DNS_CACHE_VARIANT
    PROPERTY STRINGS naive optimized
)

add_library(common_options INTERFACE)

target_compile_options(common_options
    INTERFACE
        -Wall
        -Wextra
        -Wpedantic
)

find_package(SQLite3 REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(common_options INTERFACE 
    Threads::Threads
)

if(DNS_CACHE_VARIANT STREQUAL "optimized")
    target_compile_definitions(
        common_options
        INTERFACE DNS_CACHE_VARIANT_OPTIMIZED
    )
elseif(NOT DNS_CACHE_VARIANT STREQUAL "naive")
    message(FATAL_ERROR "DNS_CACHE_VARIANT must be naive or optimized")
endif()

add_library(common_impl STATIC
    common/src/config.cpp
    common/src/utils.cpp
)

target_include_directories(common_impl PUBLIC
    common/includes
)

target_link_libraries(common_impl PUBLIC common_options)

add_executable(server
    server/src/main.cpp
    server/src/cache.cpp
    server/src/ip.cpp
    server/src/database.cpp
    server/src/materializer.cpp
    server/src/record_repository.cpp
    server/src/parse_utils.cpp
    server/src/worker.cpp
)

target_include_directories(server PRIVATE
    server/includes
)

target_link_libraries(server PRIVATE
    common_impl
    SQLite::SQLite3
)

add_executable(client
    client/src/main.cpp
)

target_link_libraries(client PRIVATE
    common_impl
)

if(BUILD_TESTING)
    add_executable(ip_test
        tests/ip_test.cpp
        server/src/ip.cpp
    )

    target_include_directories(ip_test PRIVATE
        server/includes
    )

    add_test(NAME ip_parsing COMMAND ip_test)

    add_executable(materializer_test
        tests/materializer_test.cpp
        server/src/ip.cpp
        server/src/materializer.cpp
        server/src/parse_utils.cpp
    )

    target_include_directories(materializer_test PRIVATE
        server/includes
    )

    target_link_libraries(materializer_test PRIVATE
        common_options
        SQLite::SQLite3
    )

    add_test(NAME native_materializer COMMAND materializer_test)

    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/tests/data")

    add_executable(record_repository_test
        tests/record_repository_test.cpp
        server/src/cache.cpp
        server/src/database.cpp
        server/src/ip.cpp
        server/src/materializer.cpp
        server/src/parse_utils.cpp
        server/src/record_repository.cpp
    )

    target_include_directories(record_repository_test PRIVATE
        server/includes
        common/includes
    )

    target_link_libraries(record_repository_test PRIVATE
        common_options
        SQLite::SQLite3
    )

    add_test(
        NAME record_resolution
        COMMAND record_repository_test
            "${CMAKE_CURRENT_BINARY_DIR}/tests/data/dns.db"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/data/dns.sql"
    )
endif()

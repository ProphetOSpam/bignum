cmake_minimum_required(VERSION 3.10.0)
project(big-number VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_BINARY_DIR ./target)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_STANDARD 26)

enable_testing()

add_executable(bignum-test "")

target_sources(bignum-test PRIVATE
    test/bignum-test.cpp
)

target_link_directories(bignum-test PRIVATE
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

add_test(
    NAME bignum-test
    COMMAND bignum-test
)

add_executable(bignum-time "")

target_sources(bignum-time PRIVATE
    test/bignum-time.cpp
)

target_link_directories(bignum-time PRIVATE
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

add_test(
    NAME bignum-time
    COMMAND bignum-time
)

add_executable(bignum-benchmark "")

target_sources(bignum-benchmark PRIVATE
    test/bignum-benchmark.cpp
)

target_link_directories(bignum-benchmark PRIVATE
    ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

add_test(
    NAME bignum-benchmark
    COMMAND bignum-benchmark
)

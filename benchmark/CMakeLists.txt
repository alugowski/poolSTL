# FetchContent requires cmake >=3.11
cmake_minimum_required(VERSION 3.11...3.26)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic")
    set(CMAKE_CXX_FLAGS_RELEASE "-g -O3 -DNDEBUG")
endif ()

include(FetchContent)

# Add Google Benchmark
set(BENCHMARK_ENABLE_TESTING NO)

include(FetchContent)

FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG origin/main
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(googlebenchmark)

# Check for native compiler support for parallel STL
include(CheckCXXSourceCompiles)

# CMP0067: Honor language standard in try_compile() source-file signature.
# https://cmake.org/cmake/help/latest/policy/CMP0067.html
cmake_policy(SET CMP0067 NEW)
set(CMAKE_CXX_STANDARD 17)

check_cxx_source_compiles("
#include <algorithm>
#include <execution>
#include <vector>
int main(void) {
    std::vector<int> values(10);
    std::for_each(std::execution::par, values.begin(), values.end(), [](auto v) {});
    return 0;
}
" HAVE_STD_EXECUTION_PAR)

# benchmark
add_executable(poolstl_bench
        main.cpp
        utils.hpp
        algorithm_bench.cpp
        numeric_bench.cpp
        )
target_link_libraries(poolstl_bench benchmark::benchmark poolSTL::poolSTL)
target_compile_features(poolstl_bench PUBLIC cxx_std_17)

if (HAVE_STD_EXECUTION_PAR AND ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
    # GCC and Clang require TBB
    find_package(TBB)
    if (TBB_FOUND)
        message("Found TBB")
        target_link_libraries(poolstl_bench TBB::tbb)
    else()
        message("No TBB")
        # No TBB means cannot use std::execution::par
        unset(HAVE_STD_EXECUTION_PAR)
    endif()
endif()

if (HAVE_STD_EXECUTION_PAR)
    target_compile_definitions(poolstl_bench PUBLIC POOLSTL_BENCH_STD_PAR)
endif()

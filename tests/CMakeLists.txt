# FetchContent requires cmake >=3.11
cmake_minimum_required(VERSION 3.11...3.26)
enable_testing()

# Be pedantic for clean code
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic")
endif()

# Code Coverage
option(POOLSTL_TEST_COVERAGE "Code-coverage" OFF)
if(POOLSTL_TEST_COVERAGE)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options("--coverage")
        add_link_options("-lgcov" "--coverage")
    endif()
endif()

# install Catch2
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.3.2
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

add_executable(poolstl_test poolstl_test.cpp)
target_link_libraries(poolstl_test PRIVATE Catch2::Catch2WithMain poolSTL::poolSTL)
catch_discover_tests(poolstl_test)

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
    elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        add_compile_options("--coverage")
        add_link_options("--coverage")
    endif()
endif()

# install Catch2
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.4.0
        GIT_SHALLOW TRUE
        EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)

# tests compiled with C++14 (Catch2 requires C++14)
add_executable(poolstl_test poolstl_test.cpp)
target_link_libraries(poolstl_test PRIVATE Catch2::Catch2WithMain poolSTL::poolSTL)
target_compile_definitions(poolstl_test PRIVATE CATCH_CONFIG_FAST_COMPILE)
target_compile_features(poolstl_test PUBLIC cxx_std_14)
catch_discover_tests(poolstl_test)

# tests compiled with C++17 (Some supported methods have been added to the standard library)
add_executable(poolstl_test_cpp17 poolstl_test.cpp)
target_link_libraries(poolstl_test_cpp17 PRIVATE Catch2::Catch2WithMain poolSTL::poolSTL)
target_compile_definitions(poolstl_test_cpp17 PRIVATE CATCH_CONFIG_FAST_COMPILE)
target_compile_features(poolstl_test_cpp17 PUBLIC cxx_std_17)
catch_discover_tests(poolstl_test_cpp17)

# Dedicated target to ensure C++11 builds work
add_executable(cpp11_test cpp11_test.cpp)
target_link_libraries(cpp11_test PUBLIC poolSTL::poolSTL)
target_compile_features(cpp11_test PUBLIC cxx_std_11)

# Test std::execution supplementation
# This uses only std::execution::par. On compilers with support this should result in a poolSTL no-op,
# and on ones without support it should use fallback to poolstl::par.
add_executable(supplement_test supplement_test.cpp)
target_link_libraries(supplement_test PUBLIC poolSTL::poolSTL)
target_compile_features(supplement_test PUBLIC cxx_std_17)
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    # GCC and Clang require TBB for std::execution::par
    find_package(TBB)
    if (TBB_FOUND)
        target_link_libraries(supplement_test PUBLIC TBB::tbb)
    else()
        message("No TBB")
        target_compile_definitions(supplement_test PUBLIC POOLSTL_MISSING_NEEDED_TBB)
    endif()
endif()

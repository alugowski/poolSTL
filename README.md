[![tests](https://github.com/alugowski/poolSTL/actions/workflows/tests.yml/badge.svg)](https://github.com/alugowski/poolSTL/actions/workflows/tests.yml)
[![codecov](https://codecov.io/gh/alugowski/poolSTL/branch/main/graph/badge.svg?token=zB7yN8NwUc)](https://codecov.io/gh/alugowski/poolSTL)

# poolSTL

Thread pool-based implementation of [parallel standard library algorithms](https://en.cppreference.com/w/cpp/algorithm).

Those algorithms are great, but compiler support varies.
PoolSTL is a *supplement* to fill in the support gaps, so we can use parallel algorithms now.
It is not meant as a full implementation, only the basics are expected to be covered.  Use this if:
* you only need the basics.
* to support a [compiler lacking native support](https://en.cppreference.com/w/cpp/compiler_support/17) (see "Parallel algorithms and execution policies").
* you cannot link against TBB for whatever reason.
* the [Parallel STL](https://www.intel.com/content/www/us/en/developer/articles/guide/get-started-with-parallel-stl.html) is too heavy.

Supports C++11 and higher, C++17 preferred.
Tested in CI on GCC 7+, Clang/LLVM 5+, Apple Clang, MSVC.

## Implemented Algorithms
Algorithms are added on an as-needed basis. If you need one [open an issue](https://github.com/alugowski/poolSTL/issues) or contribute a PR.

### `<algorithm>`
* [all_of](https://en.cppreference.com/w/cpp/algorithm/all_of), [any_of](https://en.cppreference.com/w/cpp/algorithm/any_of), [none_of](https://en.cppreference.com/w/cpp/algorithm/none_of)
* [copy](https://en.cppreference.com/w/cpp/algorithm/copy), [copy_n](https://en.cppreference.com/w/cpp/algorithm/copy_n)
* [fill](https://en.cppreference.com/w/cpp/algorithm/fill), [fill_n](https://en.cppreference.com/w/cpp/algorithm/fill_n)
* [find](https://en.cppreference.com/w/cpp/algorithm/find), [find_if](https://en.cppreference.com/w/cpp/algorithm/find_if), [find_if_not](https://en.cppreference.com/w/cpp/algorithm/find_if_not)
* [for_each](https://en.cppreference.com/w/cpp/algorithm/for_each), [for_each_n](https://en.cppreference.com/w/cpp/algorithm/for_each_n)
* [transform](https://en.cppreference.com/w/cpp/algorithm/transform)

### `<numeric>`
* [reduce](https://en.cppreference.com/w/cpp/algorithm/reduce)
* [transform_reduce](https://en.cppreference.com/w/cpp/algorithm/transform_reduce) (C++17 only)

All in `std::` namespace.

**Note:** All iterators must be random access.

## Usage

PoolSTL provides `poolstl::par` and `poolstl::par_pool` execution policies. Pass either one of these as the first argument
to one of the supported algorithms and your code will be parallel.

In other words, use `poolstl::par` as you would use [`std::execution::par`](https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag). Complete example:
```c++
#include <iostream>
#include <poolstl/poolstl.hpp>

int main() {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    auto sum = std::reduce(poolstl::par, v.cbegin(), v.cend());
    //                     ^^^^^^^^^^^^
    //                     Add this to make your code parallel.
    std::cout << "Sum=" << sum << std::endl;
    return 0;
}
```

### Controlling Thread Pool Size

The thread pool used by `poolstl::par` is managed internally by poolSTL. It is started on first use.

Use your own [thread pool](https://github.com/alugowski/task-thread-pool)
with `poolstl::par_pool` for full control over thread count, startup/shutdown, etc.:

```c++
task_thread_pool::task_thread_pool pool{4};  // 4 threads

std::reduce(poolstl::par_pool(pool), v.cbegin(), v.cbegin());
```

## Installation

### Single File

Copy a single-file amalgamated `poolstl.hpp` from the [latest release](https://github.com/alugowski/poolSTL/releases) and into your project.

### CMake

```cmake
include(FetchContent)
FetchContent_Declare(
        poolSTL
        GIT_REPOSITORY https://github.com/alugowski/poolSTL
        GIT_TAG main
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(poolSTL)

target_link_libraries(YOUR_TARGET poolSTL::poolSTL)
```

Alternatively copy or checkout the repo into your project and:
```cmake
add_subdirectory(poolSTL)
```

# Benchmark

See [benchmark/](benchmark) to compare poolSTL against the standard sequential implementation, and (if available) the
native `std::execution::par` implementation.

Results on an M1 Pro (6 power, 2 efficiency cores), with GCC 13:
```
-------------------------------------------------------------------------------------------------------
Benchmark                                                             Time             CPU   Iterations
-------------------------------------------------------------------------------------------------------
all_of()/real_time                                                 19.8 ms         19.8 ms           35
all_of(poolstl::par)/real_time                                     3.87 ms        0.113 ms          175
all_of(std::execution::par)/real_time                              3.84 ms         3.27 ms          198
find_if()/needle_percentile:5/real_time                            1.01 ms         1.00 ms          708
find_if()/needle_percentile:50/real_time                           9.91 ms         9.90 ms           71
find_if()/needle_percentile:100/real_time                          19.8 ms         19.7 ms           35
find_if(poolstl::par)/needle_percentile:5/real_time               0.391 ms        0.045 ms         1787
find_if(poolstl::par)/needle_percentile:50/real_time               1.83 ms        0.081 ms          353
find_if(poolstl::par)/needle_percentile:100/real_time              3.58 ms        0.085 ms          197
find_if(std::execution::par)/needle_percentile:5/real_time        0.234 ms        0.227 ms         3051
find_if(std::execution::par)/needle_percentile:50/real_time        1.87 ms         1.79 ms          377
find_if(std::execution::par)/needle_percentile:100/real_time       3.91 ms         3.51 ms          177
for_each()/real_time                                               94.8 ms         94.8 ms            7
for_each(poolstl::par)/real_time                                   20.2 ms        0.041 ms           37
for_each(std::execution::par)/real_time                            17.1 ms         14.2 ms           45
transform()/real_time                                              95.8 ms         95.8 ms            7
transform(poolstl::par)/real_time                                  20.8 ms        0.041 ms           38
transform(std::execution::par)/real_time                           16.8 ms         14.3 ms           41
reduce()/real_time                                                 15.1 ms         15.1 ms           46
reduce(poolstl::par)/real_time                                     4.21 ms        0.046 ms          165
reduce(std::execution::par)/real_time                              3.55 ms         3.09 ms          199
```

# poolSTL as `std::execution::par` Substitute
**USE AT YOUR OWN RISK!**

Two-line hack for missing compiler support. A no-op on compilers with support.

If `POOLSTL_STD_SUPPLEMENT` is defined then poolSTL will check for native compiler support.
If not found then poolSTL will alias its `poolstl::par` as `std::execution::par`:

```c++
#define POOLSTL_STD_SUPPLEMENT
#include <poolstl/poolstl.hpp>
```

Now just use `std::execution::par` as normal, and poolSTL will fill in as necessary. See [supplement_test.cpp](tests/supplement_test.cpp).

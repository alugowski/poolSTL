[![tests](https://github.com/alugowski/poolSTL/actions/workflows/tests.yml/badge.svg)](https://github.com/alugowski/poolSTL/actions/workflows/tests.yml)
[![codecov](https://codecov.io/gh/alugowski/poolSTL/branch/main/graph/badge.svg?token=zB7yN8NwUc)](https://codecov.io/gh/alugowski/poolSTL)

# poolSTL

Thread pool-based implementation of [parallel standard library algorithms](https://en.cppreference.com/w/cpp/algorithm).

Those algorithms are great, but compiler support varies.
PoolSTL is a *supplement* to fill in the support gaps, so we can use parallel algorithms now.
It is not meant as a full implementation, only the basics are expected to be covered.  Use this if:
* you only need the basics, including no nested parallel calls.
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
* [sort](https://en.cppreference.com/w/cpp/algorithm/sort), [stable_sort](https://en.cppreference.com/w/cpp/algorithm/stable_sort)
* [transform](https://en.cppreference.com/w/cpp/algorithm/transform)

### `<numeric>`
* [reduce](https://en.cppreference.com/w/cpp/algorithm/reduce)
* [transform_reduce](https://en.cppreference.com/w/cpp/algorithm/transform_reduce) (C++17 only)

All in `std::` namespace.

**Note:** All iterators must be random access.

## Usage

PoolSTL provides these execution policies:
* `poolstl::par`: Substitute for [`std::execution::par`](https://en.cppreference.com/w/cpp/algorithm/execution_policy_tag).
* `poolstl::par_pool`: `par` but with your own [thread pool](https://github.com/alugowski/task-thread-pool).
* `poolstl::seq`: Substitute for `std::execution::seq`. Simply calls the sequential (non-policy) overload.

In other words, use `poolstl::par` to make your code parallel. Complete example:
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
all_of()/real_time                                                 19.9 ms         19.9 ms           35
all_of(poolstl::par)/real_time                                     3.47 ms        0.119 ms          198
all_of(std::execution::par)/real_time                              3.45 ms         3.25 ms          213
find_if()/needle_percentile:5/real_time                           0.988 ms        0.987 ms          712
find_if()/needle_percentile:50/real_time                           9.87 ms         9.86 ms           71
find_if()/needle_percentile:100/real_time                          19.7 ms         19.7 ms           36
find_if(poolstl::par)/needle_percentile:5/real_time               0.405 ms        0.050 ms         1730
find_if(poolstl::par)/needle_percentile:50/real_time               1.85 ms        0.096 ms          393
find_if(poolstl::par)/needle_percentile:100/real_time              3.64 ms        0.102 ms          193
find_if(std::execution::par)/needle_percentile:5/real_time        0.230 ms        0.220 ms         3103
find_if(std::execution::par)/needle_percentile:50/real_time        1.75 ms         1.60 ms          410
find_if(std::execution::par)/needle_percentile:100/real_time       3.51 ms         3.24 ms          204
for_each()/real_time                                               94.6 ms         94.6 ms            7
for_each(poolstl::par)/real_time                                   18.7 ms        0.044 ms           36
for_each(std::execution::par)/real_time                            15.3 ms         12.9 ms           46
sort()/real_time                                                    603 ms          602 ms            1
sort(poolstl::par)/real_time                                        146 ms        0.667 ms            5
sort(std::execution::par)/real_time                                 121 ms         95.1 ms            6
transform()/real_time                                              95.0 ms         94.9 ms            7
transform(poolstl::par)/real_time                                  17.4 ms        0.037 ms           38
transform(std::execution::par)/real_time                           15.3 ms         13.2 ms           45
reduce()/real_time                                                 15.2 ms         15.2 ms           46
reduce(poolstl::par)/real_time                                     4.06 ms        0.044 ms          169
reduce(std::execution::par)/real_time                              3.38 ms         3.16 ms          214
```

# poolSTL as `std::execution::par`
**USE AT YOUR OWN RISK!**

Two-line hack for missing compiler support. A no-op on compilers with support.

If `POOLSTL_STD_SUPPLEMENT` is defined then poolSTL will check for native compiler support.
If not found then poolSTL will alias its `poolstl::par` as `std::execution::par`:

```c++
#define POOLSTL_STD_SUPPLEMENT
#include <poolstl/poolstl.hpp>
```

Now just use `std::execution::par` as normal, and poolSTL will fill in as necessary. See [supplement_test.cpp](tests/supplement_test.cpp).

Example use case: You *can* link against TBB, so you'll use native support on GCC 9+, Clang, MSVC, etc.
PoolSTL will fill in automatically on GCC <9 and Apple Clang.

[![tests](https://github.com/alugowski/poolSTL/actions/workflows/tests.yml/badge.svg)](https://github.com/alugowski/poolSTL/actions/workflows/tests.yml)
[![codecov](https://codecov.io/gh/alugowski/poolSTL/branch/main/graph/badge.svg?token=zB7yN8NwUc)](https://codecov.io/gh/alugowski/poolSTL)

# poolSTL

Thread pool-based implementation of [parallel standard library algorithms](https://en.cppreference.com/w/cpp/algorithm).

Those algorithms are great, but compiler support is inconsistent.
PoolSTL is a *supplement* to fill in the support gaps so we can use parallel algorithms now.

It is not meant as a full implementation, only the basics are expected to be covered.

Use this if:
* you only need the basics
* you must support a [compiler that lacks native support](https://en.cppreference.com/w/cpp/compiler_support/17) (see the "Parallel algorithms and execution policies" row)
* you cannot link against TBB for whatever reason
* the [Parallel STL](https://www.intel.com/content/www/us/en/developer/articles/guide/get-started-with-parallel-stl.html) is too heavy

Supports C++11 and higher, C++17 preferred.
Tested in CI on GCC 7+, Clang/LLVM 5+, Apple Clang, MSVC.

## Implemented algorithms
Algorithms are added on an as-needed basis. If you need one that is not present feel free to open an issue or submit a PR.

### `<algorithm>`
* [std::copy](https://en.cppreference.com/w/cpp/algorithm/copy)
* [std::fill](https://en.cppreference.com/w/cpp/algorithm/fill)
* [std::for_each](https://en.cppreference.com/w/cpp/algorithm/for_each)
* [std::for_each_n](https://en.cppreference.com/w/cpp/algorithm/for_each_n)
* [std::transform](https://en.cppreference.com/w/cpp/algorithm/transform)

### `<numeric>`
* [std::reduce](https://en.cppreference.com/w/cpp/algorithm/reduce)

Note: All iterators must be random access.

## Usage

PoolSTL defines `poolstl::par` and `poolstl::par_pool` execution policies. Pass either one of these as the first argument
to one of the supported algorithms and your code will be parallel.

In other words, use `poolstl::par` as you would use `std::execution::par`.

Complete example:
```c++
#include <iostream>
#include <poolstl/poolstl.hpp>

int main() {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    auto sum = std::reduce(poolstl::par, v.cbegin(), v.cend());
    //                     ^^^^^^^^^^^^
    //                     Just add this to make your code parallel.
    std::cout << "Sum=" << sum << std::endl;
    return 0;
}
```

### Pool control

Use `poolstl::par_pool` with your own [thread pool](https://github.com/alugowski/task-thread-pool) to have full control over thread count, thread startup/shutdown, etc.:

```c++
task_thread_pool::task_thread_pool pool;

std::for_each(poolstl::par_pool(pool), v.cbegin(), v.cbegin(), [](auto) {});
```

The pool used by `poolstl::par` is managed internally by poolSTL. It is started on first use.

## Installation

### Single File

You may download a single-file amalgamated `poolstl.hpp` from the [latest release](https://github.com/alugowski/poolSTL/releases) and simply copy into your project.

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

# poolSTL as `std::execution::par` substitute
**USE AT YOUR OWN RISK!**

Two-line fix for missing compiler support. A no-op on compilers with support.

If `POOLSTL_STD_SUPPLEMENT` is defined and native support is not found then poolSTL will alias its `poolstl::par` as `std::execution::par`:

```c++
#define POOLSTL_STD_SUPPLEMENT
#include <poolstl/poolstl.hpp>
```

Now just use `std::execution::par` as normal, and poolSTL will fill in as necessary. See [supplement_test.cpp](tests/supplement_test.cpp).

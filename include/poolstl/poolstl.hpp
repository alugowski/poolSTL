// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0
/**
 * @brief Thread pool-based implementation of parallel standard library algorithms.
 * @see https://github.com/alugowski/poolSTL
 * @author Adam Lugowski
 * @copyright Copyright (C) 2023 Adam Lugowski.
 *            Licensed under any of the following open-source licenses:
 *            BSD-2-Clause license, MIT license, Boost Software License 1.0
 *            See the LICENSE-*.txt files in the repo root for details.
 */

#ifndef POOLSTL_HPP
#define POOLSTL_HPP

#include "execution"
#include "algorithm"
#include "numeric"

/*
 * Optionally alias poolstl::par as std::execution::par to enable poolSTL to fill in for missing compiler support.
 *
 * USE AT YOUR OWN RISK!
 *
 * To do this define POOLSTL_STD_SUPPLEMENT before including poolstl.hpp.
 *
 * This aliasing will not happen if native support exists. If this autodetection fails for you:
 *   - define POOLSTL_ALLOW_SUPPLEMENT=0 to disable
 *   - define POOLSTL_FORCE_SUPPLEMENT to force enable (use with great care!)
 */
#ifndef POOLSTL_ALLOW_SUPPLEMENT
#define POOLSTL_ALLOW_SUPPLEMENT 1
#endif

#if POOLSTL_ALLOW_SUPPLEMENT && defined(POOLSTL_STD_SUPPLEMENT)

#if __cplusplus >= 201603L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201603L)
#if __has_include(<execution>)
#include <execution>
#endif
#endif

#if !defined(__cpp_lib_parallel_algorithm) || defined(POOLSTL_FORCE_SUPPLEMENT)
namespace std {
    namespace execution {
        using ::poolstl::execution::parallel_policy;
        using ::poolstl::execution::par;
    }
}

#endif
#endif

#endif

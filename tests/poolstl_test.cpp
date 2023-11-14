// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0


#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include <poolstl/poolstl.hpp>

#include "utils.hpp"

namespace ttp = task_thread_pool;

#if __cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#define HAVE_CXX17 1
#else
#define HAVE_CXX17 0
#endif

TEST_CASE("for_each", "[alg]") {
    std::atomic<int> sum{0};
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v = iota_vector(num_iters);

            for (auto is_sequential : {true, false}) {
                sum = 0;
                auto f = [&](auto) { ++sum; };
                if (is_sequential) {
                    std::for_each(v.cbegin(), v.cend(), f);
                } else {
                    std::for_each(poolstl::par_pool(pool), v.cbegin(), v.cend(), f);
                }
                REQUIRE(sum == num_iters);
            }
        }
    }
}

TEST_CASE("for_each_n", "[alg]") {
    std::atomic<int> sum{0};
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        auto v = iota_vector(*std::max_element(test_arr_sizes.cbegin(), test_arr_sizes.cend()));

        for (auto num_iters : test_arr_sizes) {
            for (auto is_sequential : {true, false}) {
                sum = 0;
                auto f = [&](auto) { ++sum; };
                if (is_sequential) {
#if HAVE_CXX17
                    std::for_each_n(v.cbegin(), num_iters, f);
#endif
                } else {
                    std::for_each_n(poolstl::par_pool(pool), v.cbegin(), num_iters, f);
                }
#if HAVE_CXX17
                REQUIRE(sum == num_iters);
#endif
            }
        }
    }
}

TEST_CASE("default_pool", "[execution]") {
    std::atomic<int> sum{0};
    for (auto num_iters : test_arr_sizes) {
        auto v = iota_vector(num_iters);
        sum = 0;
        std::for_each(poolstl::par, v.cbegin(), v.cend(), [&](auto) { ++sum; });
        REQUIRE(sum == num_iters);
    }
}

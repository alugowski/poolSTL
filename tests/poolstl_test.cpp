// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0


#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include <poolstl/poolstl.hpp>

#include "utils.hpp"

namespace ttp = task_thread_pool;

TEST_CASE("for_each", "[alg][algorithm]") {
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

TEST_CASE("for_each_n", "[alg][algorithm]") {
    std::atomic<int> sum{0};
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        auto v = iota_vector(*std::max_element(test_arr_sizes.cbegin(), test_arr_sizes.cend()));

        for (auto num_iters : test_arr_sizes) {
            sum = 0;
            auto f = [&](auto) { ++sum; };
            // sequential for_each_n has incomplete GCC7 support
            std::for_each_n(poolstl::par_pool(pool), v.cbegin(), num_iters, f);
            REQUIRE(sum == num_iters);
        }
    }
}

TEST_CASE("transform_1", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v = iota_vector(num_iters);
            std::vector<int> dest1(v.size());
            std::vector<int> dest2(v.size());

            auto unary_op = [&](auto x) { return 2*x; };

            auto par_res = std::transform(poolstl::par_pool(pool), v.cbegin(), v.cend(), dest1.begin(), unary_op);
#if POOLSTL_HAVE_CXX17_LIB
            auto seq_res = std::transform(v.cbegin(), v.cend(), dest1.begin(), unary_op);
#else
            auto seq_res = poolstl::internal::cpp17::transform(v.cbegin(), v.cend(), dest1.begin(), unary_op);
#endif
            // test return value
            REQUIRE(par_res == seq_res);

            // test transformed values
            std::transform(poolstl::par_pool(pool), v.cbegin(), v.cend(), dest2.begin(), unary_op);
            REQUIRE(dest1 == dest2);
        }
    }
}

TEST_CASE("transform_2", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v1 = iota_vector(num_iters);
            auto v2 = iota_vector(num_iters, 1000);
            std::vector<int> dest1(v1.size());
            std::vector<int> dest2(v1.size());

            auto binary_op = [&](auto a, auto b) { return a + b; };

            auto par_res = std::transform(poolstl::par_pool(pool), v1.cbegin(), v1.cend(), v2.cbegin(), dest1.begin(), binary_op);
#if POOLSTL_HAVE_CXX17_LIB
            auto seq_res = std::transform(v1.cbegin(), v1.cend(), v2.cbegin(), dest1.begin(), binary_op);
#else
            auto seq_res = poolstl::internal::cpp17::transform(v1.cbegin(), v1.cend(), v2.cbegin(), dest1.begin(), binary_op);
#endif
            // test return value
            REQUIRE(par_res == seq_res);

            // test transformed values
            std::transform(poolstl::par_pool(pool), v1.cbegin(), v1.cend(), v2.cbegin(), dest2.begin(), binary_op);
            REQUIRE(dest1 == dest2);
        }
    }
}

TEST_CASE("reduce", "[alg][numeric]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v = iota_vector(num_iters);

#if POOLSTL_HAVE_CXX17_LIB
            auto seq = std::reduce(v.cbegin(), v.cend());
#else
            auto seq = poolstl::internal::cpp17::reduce(v.cbegin(), v.cend());
#endif
            auto par = std::reduce(poolstl::par_pool(pool), v.cbegin(), v.cend());
            REQUIRE(seq == par);
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

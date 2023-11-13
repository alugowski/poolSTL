// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0


#ifndef POOLSTL_BENCH_UTILS_HPP
#define POOLSTL_BENCH_UTILS_HPP

#include <algorithm>
#include <execution>
#include <numeric>
#include <utility>
#include <vector>

#include <benchmark/benchmark.h>

#include <poolstl/algorithm>
#include <poolstl/numeric>

constexpr long long arr_length = 100'000'000;

struct seq {};
struct poolstl_par {};
struct std_par {};

template <typename T> struct is_policy : std::true_type {};
template <> struct is_policy<seq> : std::false_type {};


template <typename T> struct policy {};
template <> struct policy<poolstl_par> {
    constexpr static poolstl::execution::parallel_policy get() {
        return poolstl::execution::par;
    };
};
template <> struct policy<poolstl::execution::par_pool> {
    static poolstl::execution::par_pool get() {
        static task_thread_pool::task_thread_pool pool;
        return poolstl::execution::par_pool(pool);
    };
};
template <> struct policy<std_par> {
    constexpr static const std::execution::parallel_policy& get() {
        return std::execution::par;
    };
};

template <typename T=int>
std::vector<T> iota_vector(size_t size, T init=0) {
    std::vector<T> ret(size);
    std::iota(ret.begin(), ret.end(), init);
    return ret;
}

#endif

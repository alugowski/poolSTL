// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0


#include <algorithm>
#include <vector>

#include <benchmark/benchmark.h>

#include "utils.hpp"
#include <poolstl/algorithm>

////////////////////////////////

template <class ExecutionPolicy>
void all_of(benchmark::State& state) {
    auto values = iota_vector(arr_length);

    for ([[maybe_unused]] auto _ : state) {
        bool res;
        if constexpr (is_policy<ExecutionPolicy>::value) {
            res = std::all_of(policy<ExecutionPolicy>::get(), values.begin(), values.end(), [&](auto v) { return v >= 0; });
        } else {
            res = std::all_of(values.begin(), values.end(), [&](auto v) { return v >= 0; });
        }
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(all_of<seq>)->UseRealTime();
//BENCHMARK(all_of<poolstl::par>)->UseRealTime();
#ifdef POOLSTL_BENCH_STD_PAR
BENCHMARK(all_of<std_par>)->UseRealTime();
#endif

////////////////////////////////

template <class ExecutionPolicy>
void for_each(benchmark::State& state) {
    auto values = iota_vector<int>(arr_length);
    std::vector<int> dest(arr_length);

    for ([[maybe_unused]] auto _ : state) {
        if constexpr (is_policy<ExecutionPolicy>::value) {
            std::for_each(policy<ExecutionPolicy>::get(), values.begin(), values.end(), [&](auto v) { slow(); dest[v] = v; });
        } else {
            std::for_each(values.begin(), values.end(), [&](auto v) {slow(); dest[v] = v;});
        }
        benchmark::DoNotOptimize(dest);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(for_each<seq>)->UseRealTime();
BENCHMARK(for_each<poolstl_par>)->UseRealTime();
#ifdef POOLSTL_BENCH_STD_PAR
BENCHMARK(for_each<std_par>)->UseRealTime();
#endif

////////////////////////////////

template <class ExecutionPolicy>
void transform(benchmark::State& state) {
    auto values = iota_vector<int>(arr_length);
    std::vector<int> dest(arr_length);

    for ([[maybe_unused]] auto _ : state) {
        if constexpr (is_policy<ExecutionPolicy>::value) {
            std::transform(policy<ExecutionPolicy>::get(), values.begin(), values.end(), dest.begin(), [&](auto v) { slow(); return v; });
        } else {
            std::transform(values.begin(), values.end(), dest.begin(), [&](auto v) { slow(); return v; });
        }
        benchmark::DoNotOptimize(dest);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(transform<seq>)->UseRealTime();
//BENCHMARK(transform<poolstl::par>)->UseRealTime();
#ifdef POOLSTL_BENCH_STD_PAR
BENCHMARK(transform<std_par>)->UseRealTime();
#endif

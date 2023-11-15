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

template <class ExecPolicy>
void all_of(benchmark::State& state) {
    auto values = iota_vector(arr_length);

    for ([[maybe_unused]] auto _ : state) {
        bool res;
        if constexpr (is_policy<ExecPolicy>::value) {
            res = std::all_of(policy<ExecPolicy>::get(), values.begin(), values.end(), [&](auto v) { return v >= 0; });
        } else {
            res = std::all_of(values.begin(), values.end(), [&](auto v) { return v >= 0; });
        }
        benchmark::DoNotOptimize(res);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(all_of<seq>)->Name("all_of()")->UseRealTime();
//BENCHMARK(all_of<poolstl_par>)->Name("all_of(poolstl::par)")->UseRealTime();
#ifdef POOLSTL_BENCH_STD_PAR
BENCHMARK(all_of<std_par>)->Name("all_of(std::execution::par)")->UseRealTime();
#endif

////////////////////////////////

template <class ExecPolicy>
void for_each(benchmark::State& state) {
    auto values = iota_vector<int>(arr_length);
    std::vector<int> dest(arr_length);

    for ([[maybe_unused]] auto _ : state) {
        if constexpr (is_policy<ExecPolicy>::value) {
            std::for_each(policy<ExecPolicy>::get(), values.begin(), values.end(), [&](auto v) { slow(); dest[v] = v; });
        } else {
            std::for_each(values.begin(), values.end(), [&](auto v) {slow(); dest[v] = v;});
        }
        benchmark::DoNotOptimize(dest);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(for_each<seq>)->Name("for_each()")->UseRealTime();
BENCHMARK(for_each<poolstl_par>)->Name("for_each(poolstl::par)")->UseRealTime();
#ifdef POOLSTL_BENCH_STD_PAR
BENCHMARK(for_each<std_par>)->Name("for_each(std::execution::par)")->UseRealTime();
#endif

////////////////////////////////

template <class ExecPolicy>
void transform(benchmark::State& state) {
    auto values = iota_vector<int>(arr_length);
    std::vector<int> dest(arr_length);

    for ([[maybe_unused]] auto _ : state) {
        if constexpr (is_policy<ExecPolicy>::value) {
            std::transform(policy<ExecPolicy>::get(), values.begin(), values.end(), dest.begin(), [&](auto v) { slow(); return v; });
        } else {
            std::transform(values.begin(), values.end(), dest.begin(), [&](auto v) { slow(); return v; });
        }
        benchmark::DoNotOptimize(dest);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(transform<seq>)->Name("transform()")->UseRealTime();
BENCHMARK(transform<poolstl_par>)->Name("transform(poolstl::par)")->UseRealTime();
#ifdef POOLSTL_BENCH_STD_PAR
BENCHMARK(transform<std_par>)->Name("transform(std::execution::par)")->UseRealTime();
#endif

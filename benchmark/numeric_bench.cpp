// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0


#include <numeric>
#include <vector>

#include <benchmark/benchmark.h>

#include <poolstl/numeric>

#include "utils.hpp"


////////////////////////////////

template <class ExecutionPolicy>
void reduce(benchmark::State& state) {
    auto values = iota_vector(arr_length);

    for ([[maybe_unused]] auto _ : state) {
        int64_t sum = 0;
        if constexpr (is_policy<ExecutionPolicy>::value) {
            sum = std::reduce(policy<ExecutionPolicy>::get(), values.begin(), values.end());
        } else {
            sum = std::reduce(values.begin(), values.end());
        }
        benchmark::DoNotOptimize(sum);
        benchmark::ClobberMemory();
    }
}

BENCHMARK(reduce<seq>)->UseRealTime();
//BENCHMARK(reduce<poolstl::par>)->UseRealTime();
#ifdef POOLSTL_BENCH_STD_PAR
BENCHMARK(reduce<std_par>)->UseRealTime();
#endif

////////////////////////////////

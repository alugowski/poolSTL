// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0


#include "utils.hpp"

bool static_init() {
    benchmark::SetDefaultTimeUnit(benchmark::kMillisecond);
    // init the poolSTL thread pool
    poolstl::internal::get_default_pool();
    return true;
}
static bool initialized = static_init();

BENCHMARK_MAIN();

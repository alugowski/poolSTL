// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0

#ifndef POOLSTL_INTERNAL_UTILS_HPP
#define POOLSTL_INTERNAL_UTILS_HPP

// Version macros.
#define POOLSTL_VERSION_MAJOR 0
#define POOLSTL_VERSION_MINOR 1
#define POOLSTL_VERSION_PATCH 0

#include <cstddef>

namespace poolstl::internal {

    template<typename Iterator>
    std::size_t get_chunk_size(Iterator first, Iterator last, unsigned int num_threads) {
        std::size_t num_steps = (last - first);
        auto remainder = num_steps % num_threads;
        return (num_steps / num_threads) + (remainder > 0 ? 1 : 0);
    }

}

#endif

// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0

#ifndef POOLSTL_INTERNAL_TTP_IMPL_HPP
#define POOLSTL_INTERNAL_TTP_IMPL_HPP

#include <future>
#include <numeric>
#include <vector>

#include "utils.hpp"

namespace poolstl {
    namespace internal {

        template <class ExecPolicy, class RandIt, class Chunk>
        std::vector<std::future<decltype(std::declval<Chunk>()(std::declval<RandIt>(), std::declval<RandIt>()))>>
        parallel_chunk_for(ExecPolicy &&policy, RandIt first, RandIt last, Chunk chunk) {
            std::vector<std::future<
                decltype(std::declval<Chunk>()(std::declval<RandIt>(), std::declval<RandIt>()))
                >> futures;
            auto chunk_size = get_chunk_size(first, last, pool(policy).get_num_threads());

            while (first < last) {
                RandIt loop_end = chunk_advance(first, last, chunk_size);

                futures.emplace_back(pool(policy).submit(chunk, first, loop_end));

                first = loop_end;
            }

            return futures;
        }

    }
}

#endif

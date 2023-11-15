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

        /**
         * Chunk a single range.
         */
        template <class ExecPolicy, class RandIt, class Chunk>
        std::vector<std::future<decltype(std::declval<Chunk>()(std::declval<RandIt>(), std::declval<RandIt>()))>>
        parallel_chunk_for(ExecPolicy &&policy, RandIt first, RandIt last, Chunk chunk, int extra_split_factor = 1) {
            std::vector<std::future<
                decltype(std::declval<Chunk>()(std::declval<RandIt>(), std::declval<RandIt>()))
                >> futures;
            auto chunk_size = get_chunk_size(first, last, extra_split_factor * pool(policy).get_num_threads());

            while (first < last) {
                auto iter_chunk_size = get_iter_chunk_size(first, last, chunk_size);
                RandIt loop_end = advanced(first, iter_chunk_size);

                futures.emplace_back(pool(policy).submit(chunk, first, loop_end));

                first = loop_end;
            }

            return futures;
        }

        /**
         * Element-wise chunk two ranges.
         */
        template <class ExecPolicy, class RandIt1, class RandIt2, class Chunk>
        std::vector<std::future<decltype(std::declval<Chunk>()(
            std::declval<RandIt1>(),
            std::declval<RandIt1>(),
            std::declval<RandIt2>()))>>
        parallel_chunk_for(ExecPolicy &&policy, RandIt1 first1, RandIt1 last1, RandIt2 first2, Chunk chunk) {
            std::vector<std::future<decltype(std::declval<Chunk>()(
                    std::declval<RandIt1>(),
                    std::declval<RandIt1>(),
                    std::declval<RandIt2>()))
            >> futures;
            auto chunk_size = get_chunk_size(first1, last1, pool(policy).get_num_threads());

            while (first1 < last1) {
                auto iter_chunk_size = get_iter_chunk_size(first1, last1, chunk_size);
                RandIt1 loop_end = advanced(first1, iter_chunk_size);

                futures.emplace_back(pool(policy).submit(chunk, first1, loop_end, first2));

                first1 = loop_end;
                std::advance(first2, iter_chunk_size);
            }

            return futures;
        }

        /**
         * Element-wise chunk three ranges.
         */
        template <class ExecPolicy, class RandIt1, class RandIt2, class RandIt3, class Chunk>
        std::vector<std::future<decltype(std::declval<Chunk>()(
            std::declval<RandIt1>(),
            std::declval<RandIt1>(),
            std::declval<RandIt2>(),
            std::declval<RandIt3>()))>>
        parallel_chunk_for(ExecPolicy &&policy, RandIt1 first1, RandIt1 last1, RandIt2 first2, RandIt3 first3, Chunk chunk) {
            std::vector<std::future<decltype(std::declval<Chunk>()(
                std::declval<RandIt1>(),
                std::declval<RandIt1>(),
                std::declval<RandIt2>(),
                std::declval<RandIt3>()))
            >> futures;
            auto chunk_size = get_chunk_size(first1, last1, pool(policy).get_num_threads());

            while (first1 < last1) {
                auto iter_chunk_size = get_iter_chunk_size(first1, last1, chunk_size);
                RandIt1 loop_end = advanced(first1, iter_chunk_size);

                futures.emplace_back(pool(policy).submit(chunk, first1, loop_end, first2, first3));

                first1 = loop_end;
                std::advance(first2, iter_chunk_size);
                std::advance(first3, iter_chunk_size);
            }

            return futures;
        }

    }
}

#endif

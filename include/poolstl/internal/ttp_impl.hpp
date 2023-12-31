// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0

#ifndef POOLSTL_INTERNAL_TTP_IMPL_HPP
#define POOLSTL_INTERNAL_TTP_IMPL_HPP

#include <algorithm>
#include <future>
#include <numeric>
#include <utility>
#include <vector>

#include "utils.hpp"
#include "../execution"

namespace poolstl {
    namespace internal {

#if POOLSTL_HAVE_CXX17_LIB
        /**
         * Call std::apply in parallel.
         */
        template <class ExecPolicy, class Op, class ArgContainer>
        std::vector<std::future<void>>
        parallel_apply(ExecPolicy &&policy, Op op, const ArgContainer& args_list) {
            std::vector<std::future<void>> futures;
            auto& task_pool = *policy.pool();

            for (const auto& args : args_list) {
                futures.emplace_back(task_pool.submit([](Op op, const auto& args_fwd) {
                        std::apply(op, args_fwd);
                    }, op, args));
            }

            return futures;
        }
#endif

        /**
         * Chunk a single range, with autodetected return types.
         */
        template <class ExecPolicy, class RandIt, class Chunk,
            class ChunkRet = decltype(std::declval<Chunk>()(std::declval<RandIt>(), std::declval<RandIt>()))>
        std::vector<std::future<ChunkRet>>
        parallel_chunk_for_gen(ExecPolicy &&policy, RandIt first, RandIt last, Chunk chunk,
                               ChunkRet* = (decltype(std::declval<Chunk>()(std::declval<RandIt>(),
                                                     std::declval<RandIt>()))*)nullptr,
                               int extra_split_factor = 1) {
            std::vector<std::future<ChunkRet>> futures;
            auto& task_pool = *policy.pool();
            auto chunk_size = get_chunk_size(first, last, extra_split_factor * task_pool.get_num_threads());

            while (first < last) {
                auto iter_chunk_size = get_iter_chunk_size(first, last, chunk_size);
                RandIt loop_end = advanced(first, iter_chunk_size);

                futures.emplace_back(task_pool.submit(std::forward<Chunk>(chunk), first, loop_end));

                first = loop_end;
            }

            return futures;
        }

        /**
         * Chunk a single range.
         */
        template <class ExecPolicy, class RandIt, class Chunk, class ChunkRet, typename... A>
        std::vector<std::future<ChunkRet>>
        parallel_chunk_for_1(ExecPolicy &&policy, RandIt first, RandIt last,
                             Chunk chunk, ChunkRet*, int extra_split_factor, A&&... chunk_args) {
            std::vector<std::future<ChunkRet>> futures;
            auto& task_pool = *policy.pool();
            auto chunk_size = get_chunk_size(first, last, extra_split_factor * task_pool.get_num_threads());

            while (first < last) {
                auto iter_chunk_size = get_iter_chunk_size(first, last, chunk_size);
                RandIt loop_end = advanced(first, iter_chunk_size);

                futures.emplace_back(task_pool.submit(std::forward<Chunk>(chunk), first, loop_end,
                                                      std::forward<A>(chunk_args)...));

                first = loop_end;
            }

            return futures;
        }

        /**
         * Chunk a single range.
         */
        template <class ExecPolicy, class RandIt, class Chunk, class ChunkRet, typename... A>
        typename std::enable_if<!is_pure_threads_policy<ExecPolicy>::value, void>::type
        parallel_chunk_for_1_wait(ExecPolicy &&policy, RandIt first, RandIt last,
                                  Chunk chunk, ChunkRet* rettype, int extra_split_factor, A&&... chunk_args) {
            auto futures = parallel_chunk_for_1(std::forward<ExecPolicy>(policy), first, last,
                                                std::forward<Chunk>(chunk), rettype, extra_split_factor,
                                                std::forward<A>(chunk_args)...);
            get_futures(futures);
        }

        /**
         * Element-wise chunk two ranges.
         */
        template <class ExecPolicy, class RandIt1, class RandIt2, class Chunk, class ChunkRet, typename... A>
        std::vector<std::future<ChunkRet>>
        parallel_chunk_for_2(ExecPolicy &&policy, RandIt1 first1, RandIt1 last1, RandIt2 first2,
                             Chunk chunk, ChunkRet*, A&&... chunk_args) {
            std::vector<std::future<ChunkRet>> futures;
            auto& task_pool = *policy.pool();
            auto chunk_size = get_chunk_size(first1, last1, task_pool.get_num_threads());

            while (first1 < last1) {
                auto iter_chunk_size = get_iter_chunk_size(first1, last1, chunk_size);
                RandIt1 loop_end = advanced(first1, iter_chunk_size);

                futures.emplace_back(task_pool.submit(std::forward<Chunk>(chunk), first1, loop_end, first2,
                                                      std::forward<A>(chunk_args)...));

                first1 = loop_end;
                std::advance(first2, iter_chunk_size);
            }

            return futures;
        }

        /**
         * Element-wise chunk three ranges.
         */
        template <class ExecPolicy, class RandIt1, class RandIt2, class RandIt3,
                  class Chunk, class ChunkRet, typename... A>
        std::vector<std::future<ChunkRet>>
        parallel_chunk_for_3(ExecPolicy &&policy, RandIt1 first1, RandIt1 last1, RandIt2 first2, RandIt3 first3,
                           Chunk chunk, ChunkRet*, A&&... chunk_args) {
            std::vector<std::future<ChunkRet>> futures;
            auto& task_pool = *policy.pool();
            auto chunk_size = get_chunk_size(first1, last1, task_pool.get_num_threads());

            while (first1 < last1) {
                auto iter_chunk_size = get_iter_chunk_size(first1, last1, chunk_size);
                RandIt1 loop_end = advanced(first1, iter_chunk_size);

                futures.emplace_back(task_pool.submit(std::forward<Chunk>(chunk), first1, loop_end, first2, first3,
                                                      std::forward<A>(chunk_args)...));

                first1 = loop_end;
                std::advance(first2, iter_chunk_size);
                std::advance(first3, iter_chunk_size);
            }

            return futures;
        }

        /**
         * Sort a range in parallel.
         *
         * @param stable Whether to use std::stable_sort or std::sort
         */
        template <class ExecPolicy, class RandIt, class Compare, class SortFunc>
        void parallel_sort(ExecPolicy &&policy, RandIt first, RandIt last, Compare comp, SortFunc sortfunc) {
            if (first == last) {
                return;
            }

            // Sort chunks in parallel
            auto futures = parallel_chunk_for_gen(std::forward<ExecPolicy>(policy), first, last,
                             [&comp, sortfunc] (RandIt chunk_first, RandIt chunk_last) {
                                 sortfunc(chunk_first, chunk_last, comp);
                                 return std::make_pair(chunk_first, chunk_last);
                             });

            // Merge the sorted ranges
            using SortedRange = std::pair<RandIt, RandIt>;
            auto& task_pool = *policy.pool();
            std::vector<SortedRange> subranges;
            do {
                for (auto& future : futures) {
                    subranges.emplace_back(future.get());
                }
                futures.clear();

                for (std::size_t i = 0; i < subranges.size(); ++i) {
                    if (i + 1 < subranges.size()) {
                        // pair up and merge
                        auto& lhs = subranges[i];
                        auto& rhs = subranges[i + 1];
                        futures.emplace_back(task_pool.submit([&comp] (RandIt chunk_first, RandIt chunk_middle,
                                                                       RandIt chunk_last) {
                            std::inplace_merge(chunk_first, chunk_middle, chunk_last, comp);
                            return std::make_pair(chunk_first, chunk_last);
                        }, lhs.first, lhs.second, rhs.second));
                        ++i;
                    } else {
                        // forward the final extra range
                        std::promise<SortedRange> p;
                        futures.emplace_back(p.get_future());
                        p.set_value(subranges[i]);
                    }
                }

                subranges.clear();
            } while (futures.size() > 1);
            futures.front().get();
        }
    }
}

#endif

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

        template <class ExecPolicy, class RandIt, class UnaryFunction>
        void parallel_for(ExecPolicy &&policy, RandIt first, RandIt last, UnaryFunction f) {
            std::vector<std::future<void>> futures;
            auto chunk_size = get_chunk_size(first, last, pool(policy).get_num_threads());

            while (first < last) {
                RandIt loop_end = chunk_advance(first, last, chunk_size);

                futures.emplace_back(pool(policy).submit([&f](RandIt chunk_first, RandIt chunk_last) {
                    for (; chunk_first != chunk_last; ++chunk_first) {
                        f(*chunk_first);
                    }
                }, first, loop_end));

                first = loop_end;
            }

            for (auto &future: futures) {
                future.get();
            }
        }

        template <class ExecPolicy, class RandIt, class T, class BinaryOp>
        T parallel_reduce(ExecPolicy &&policy, RandIt first, RandIt last, T init, BinaryOp binop) {
            std::vector<std::future<T>> futures;
            auto chunk_size = get_chunk_size(first, last, pool(policy).get_num_threads());

            while (first < last) {
                RandIt loop_end = chunk_advance(first, last, chunk_size);

                futures.emplace_back(pool(policy).submit([init, binop](RandIt chunk_first, RandIt chunk_last) {
                    return cpp17::reduce(chunk_first, chunk_last, init, binop);
                }, first, loop_end));

                first = loop_end;
            }

            return cpp17::reduce(get_wrap(futures.begin()), get_wrap(futures.end()), init, binop);
        }
    }
}

#endif

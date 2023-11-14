// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0

#ifndef POOLSTL_INTERNAL_TTP_IMPL_HPP
#define POOLSTL_INTERNAL_TTP_IMPL_HPP

#include <future>
#include <vector>

#include "utils.hpp"

namespace poolstl {
    namespace internal {

        template <class ExecutionPolicy, class InputIt, class UnaryFunction>
        void parallel_for(ExecutionPolicy &&policy, InputIt first, InputIt last, UnaryFunction f) {
            ::std::vector<::std::future<void>> futures;
            auto chunk_size = get_chunk_size(first, last, pool(policy).get_num_threads());

            while (first < last) {
                InputIt loop_end = chunk_advance(first, last, chunk_size);

                futures.emplace_back(pool(policy).submit([&f](InputIt chunk_first, InputIt chunk_last) {
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
    }
}

#endif

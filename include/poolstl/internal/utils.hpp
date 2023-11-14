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
#include <iterator>

namespace poolstl {
    namespace internal {

        inline constexpr std::size_t get_chunk_size(std::size_t num_steps, unsigned int num_threads) {
            auto remainder = num_steps % num_threads;
            return (num_steps / num_threads) + (remainder > 0 ? 1 : 0);
        }

        template<typename Iterator>
        constexpr typename std::iterator_traits<Iterator>::difference_type
        get_chunk_size(Iterator first, Iterator last, unsigned int num_threads) {
            using diff_t = typename std::iterator_traits<Iterator>::difference_type;
            std::size_t num_steps = std::distance(first, last);
            return static_cast<diff_t>(get_chunk_size(num_steps, num_threads));
        }

        template<typename Iterator>
        constexpr Iterator chunk_advance(const Iterator& iter, const Iterator& last,
                               typename std::iterator_traits<Iterator>::difference_type chunk_size) {
            Iterator chunk_end = iter;
            std::advance(chunk_end, ::std::min(chunk_size, std::distance(iter, last)));
            return chunk_end;
        }
    }
}

#endif

// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0

#ifndef POOLSTL_SEQ_FWD_HPP
#define POOLSTL_SEQ_FWD_HPP

#include "execution"

/*
 * Forward poolstl::seq to the native sequential (no policy) method.
 */

#define POOLSTL_DEFINE_SEQ_FWD(FNAME)                                                                       \
    template<class EP, typename...ARGS>                                                                     \
    auto FNAME(EP&&, ARGS&&...args) ->                                                                      \
                poolstl::internal::enable_if_seq<EP, decltype(std::FNAME(std::forward<ARGS>(args)...))> {   \
        return std::FNAME(std::forward<ARGS>(args)...);                                                     \
    }

#define POOLSTL_DEFINE_SEQ_FWD_VOID(FNAME)                                       \
    template<class EP, typename...ARGS>                                          \
    poolstl::internal::enable_if_seq<EP, void> FNAME(EP&&, ARGS&&... args) {     \
        std::FNAME(std::forward<ARGS>(args)...);                                 \
    }

#if POOLSTL_HAVE_CXX17

/*
 * Dynamically choose policy from a std::variant.
 * Useful to choose between parallel and sequential policies at runtime via par_if.
 */

#define POOLSTL_DEFINE_PAR_IF_FWD_VOID(FNAME)                                                             \
    template<class EP, typename...ARGS>                                                                   \
    poolstl::internal::enable_if_poolstl_variant<EP, void> FNAME(EP&& policy, ARGS&&...args) {            \
        std::visit([&](auto&& pol) { std::FNAME(pol, std::forward<ARGS>(args)...); }, policy.var);        \
    }

#define POOLSTL_DEFINE_PAR_IF_FWD(FNAME)                                                                              \
    template<class EP, typename...ARGS>                                                                               \
    auto FNAME(EP&& policy, ARGS&&...args) ->                                                                         \
                poolstl::internal::enable_if_poolstl_variant<EP, decltype(std::FNAME(std::forward<ARGS>(args)...))> { \
        return std::visit([&](auto&& pol) { return std::FNAME(pol, std::forward<ARGS>(args)...); }, policy.var);      \
    }

#else
#define POOLSTL_DEFINE_PAR_IF_FWD_VOID(FNAME)
#define POOLSTL_DEFINE_PAR_IF_FWD(FNAME)
#endif
/*
 * Define both the sequential forward and dynamic chooser.
 */
#define POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(FNAME)        \
                    POOLSTL_DEFINE_SEQ_FWD(FNAME)            \
                    POOLSTL_DEFINE_PAR_IF_FWD(FNAME)

#define POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF_VOID(FNAME)   \
                    POOLSTL_DEFINE_SEQ_FWD_VOID(FNAME)       \
                    POOLSTL_DEFINE_PAR_IF_FWD_VOID(FNAME)

namespace std {
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(all_of)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(any_of)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(none_of)

    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(count)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(count_if)

    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(copy)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(copy_n)

    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF_VOID(fill)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(fill_n)

    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(find)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(find_if)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(find_if_not)

    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF_VOID(for_each)
#if POOLSTL_HAVE_CXX17_LIB
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(for_each_n)
#endif

    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(transform)

#if POOLSTL_HAVE_CXX17_LIB
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(exclusive_scan)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(reduce)
    POOLSTL_DEFINE_BOTH_SEQ_FWD_AND_PAR_IF(transform_reduce)
#endif
}

#endif

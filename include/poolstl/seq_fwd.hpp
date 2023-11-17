// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0

#ifndef POOLSTL_SEQ_FWD_HPP
#define POOLSTL_SEQ_FWD_HPP

#include "execution"

namespace poolstl {
    namespace execution {
        struct sequential_policy {};
        constexpr sequential_policy seq{};
    }

    using execution::seq;

    namespace internal {
        // short name for short lines below
        template <class ExecPolicy, class Tp>
        using ifseq =
            typename std::enable_if<
                std::is_same<poolstl::execution::sequential_policy,
                    typename std::remove_cv<typename std::remove_reference<ExecPolicy>::type>::type>::value,
                Tp>::type;

    }
}

namespace std {

    /*
     * Forward poolstl::seq to the native sequential (no policy) method.
     */

#define POOLSTL_DEFINE_SEQ_FWD(FNAME)                                                                                  \
    template<class EP, typename...ARGS>                                                                                \
    auto FNAME(EP&&, ARGS&&...args) -> poolstl::internal::ifseq<EP,decltype(std::FNAME(std::forward<ARGS>(args)...))> {\
        return std::FNAME(std::forward<ARGS>(args)...);                                                                \
    }

#define POOLSTL_DEFINE_SEQ_FWD_VOID(FNAME)                              \
    template<class EP, typename...ARGS>                                 \
    poolstl::internal::ifseq<EP, void> FNAME(EP&&, ARGS&&...args) {     \
        std::FNAME(std::forward<ARGS>(args)...);                        \
    }

    POOLSTL_DEFINE_SEQ_FWD(all_of)
    POOLSTL_DEFINE_SEQ_FWD(any_of)
    POOLSTL_DEFINE_SEQ_FWD(none_of)

    POOLSTL_DEFINE_SEQ_FWD(copy)
    POOLSTL_DEFINE_SEQ_FWD(copy_n)

    POOLSTL_DEFINE_SEQ_FWD_VOID(fill)
    POOLSTL_DEFINE_SEQ_FWD(fill_n)

    POOLSTL_DEFINE_SEQ_FWD(find)
    POOLSTL_DEFINE_SEQ_FWD(find_if)
    POOLSTL_DEFINE_SEQ_FWD(find_if_not)

    POOLSTL_DEFINE_SEQ_FWD_VOID(for_each)
#if POOLSTL_HAVE_CXX17_LIB
    POOLSTL_DEFINE_SEQ_FWD(for_each_n)
#endif

    POOLSTL_DEFINE_SEQ_FWD(transform)

#if POOLSTL_HAVE_CXX17_LIB
    POOLSTL_DEFINE_SEQ_FWD(reduce)
    POOLSTL_DEFINE_SEQ_FWD(transform_reduce)
#endif
}

#endif

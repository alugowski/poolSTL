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
#include <functional>
#include <iterator>

#if (__cplusplus >= 201703L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)) \
    && (!defined(_GLIBCXX_RELEASE) || _GLIBCXX_RELEASE >= 9)
#define POOLSTL_HAVE_CXX17_LIB 1
#else
#define POOLSTL_HAVE_CXX17_LIB 0
#endif

#if __cplusplus >= 201402L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L)
#define POOLSTL_HAVE_CXX14 1
#else
#define POOLSTL_HAVE_CXX14 0
#endif

namespace poolstl {
    namespace internal {

        inline constexpr std::size_t get_chunk_size(std::size_t num_steps, unsigned int num_threads) {
            return (num_steps / num_threads) + ((num_steps % num_threads) > 0 ? 1 : 0);
        }

        template<typename Iterator>
        constexpr typename std::iterator_traits<Iterator>::difference_type
        get_chunk_size(Iterator first, Iterator last, unsigned int num_threads) {
            using diff_t = typename std::iterator_traits<Iterator>::difference_type;
            return static_cast<diff_t>(get_chunk_size((std::size_t)std::distance(first, last), num_threads));
        }

        template<typename Iterator>
        Iterator chunk_advance(const Iterator& iter, const Iterator& last,
                               typename std::iterator_traits<Iterator>::difference_type chunk_size) {
            Iterator chunk_end = iter;
            std::advance(chunk_end, std::min(chunk_size, std::distance(iter, last)));
            return chunk_end;
        }

        /**
         * An iterator wrapper that calls std::future<>::get().
         * @tparam Iterator
         */
        template<typename Iterator>
        class getting_iter : public Iterator {
        public:
            using value_type = decltype((*std::declval<Iterator>()).get());
            using difference_type = typename std::iterator_traits<Iterator>::difference_type;
            using pointer = value_type*;
            using reference = value_type&;
            explicit getting_iter(Iterator iter) : iter(iter) {}

            getting_iter operator++() { ++iter; return *this; }
            getting_iter operator++(int) { getting_iter ret(*this); ++iter; return ret; }

            value_type operator*() { return (*iter).get(); }
            value_type operator[](difference_type offset) { return iter[offset].get(); }

            bool operator==(const getting_iter<Iterator> &other) const { return iter == other.iter; }
            bool operator!=(const getting_iter<Iterator> &other) const { return iter != other.iter; }

        protected:
            Iterator iter;
        };

        template<typename Iterator>
        getting_iter<Iterator> get_wrap(Iterator iter) {
            return getting_iter<Iterator>(iter);
        }

        /*
         * Some methods are only available with C++17 and up. Reimplement on older standards.
         */
#if POOLSTL_HAVE_CXX17_LIB
        namespace cpp17 = std;
#else
        namespace cpp17 {
            template <class InputIt, class Tp, class BinOp>
            Tp reduce(InputIt first, InputIt last, Tp init, BinOp b) {
                for (; first != last; ++first)
                    init = b(init, *first);
                return init;
            }
            template <class InputIt>
            typename std::iterator_traits<InputIt>::value_type reduce(InputIt first, InputIt last) {
                return reduce(first, last,
                              typename std::iterator_traits<InputIt>::value_type{},
                              std::plus<typename std::iterator_traits<InputIt>::value_type>());
            }
        }
#endif
    }
}

#endif

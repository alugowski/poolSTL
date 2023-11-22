// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0


#include <algorithm>
#include <iostream>

#include <catch2/catch_test_macros.hpp>

#include <poolstl/poolstl.hpp>
#include <poolstl/iota_iter.hpp>

#include "utils.hpp"

namespace ttp = task_thread_pool;
using poolstl::iota_iter;


namespace Catch {
    template<>
    struct StringMaker<iota_iter<long>> {
        static std::string convert( iota_iter<long> const& value ) {
            return std::to_string(*value);
        }
    };
}

TEST_CASE("any_all_none", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto vec_size : test_arr_sizes) {
            auto haystack = iota_vector(vec_size);
            auto half = vec_size / 2;

            for (int which = 0; which <= 5; ++which) {
                auto pred = [&](auto x) -> bool {
                    switch (which) {
                        case 0: return x < half;
                        case 1: return x > half;
                        case 2: return x == 1;
                        case 3: return true;
                        case 4: return false;
                        default: return x == -1;
                    }
                };

                {
                    auto seq = std::any_of(poolstl::par_if<>(false), haystack.cbegin(), haystack.cend(), pred);
                    auto par = std::any_of(poolstl::par.on(pool),    haystack.cbegin(), haystack.cend(), pred);
                    REQUIRE(seq == par);
                }
                {
                    auto seq = std::all_of(poolstl::par_if<>(false), haystack.cbegin(), haystack.cend(), pred);
                    auto par = std::all_of(poolstl::par.on(pool),    haystack.cbegin(), haystack.cend(), pred);
                    REQUIRE(seq == par);
                }
                {
                    auto seq = std::none_of(poolstl::par_if<>(false), haystack.cbegin(), haystack.cend(), pred);
                    auto par = std::none_of(poolstl::par.on(pool),    haystack.cbegin(), haystack.cend(), pred);
                    REQUIRE(seq == par);
                }
            }
        }
    }
}

TEST_CASE("copy", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto source = iota_vector(num_iters);
            std::vector<int> dest1(source.size());
            std::vector<int> dest2(source.size());

            std::copy(poolstl::par_if<>(false), source.cbegin(), source.cend(), dest1.begin());
            std::copy(poolstl::par_if<>(true),  source.cbegin(), source.cend(), dest2.begin());

            REQUIRE(dest1 == dest2);
        }
    }
}

TEST_CASE("copy_n", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        auto vec_size = *std::max_element(test_arr_sizes.cbegin(), test_arr_sizes.cend());
        for (auto num_iters : test_arr_sizes) {
            auto source = iota_vector(num_iters);
            std::vector<int> dest1(vec_size);
            std::vector<int> dest2(vec_size);

            std::copy_n(poolstl::par_if<>(false), source.cbegin(), num_iters, dest1.begin());
            std::copy_n(poolstl::par.on(pool),    source.cbegin(), num_iters, dest2.begin());

            REQUIRE(dest1 == dest2);
        }
    }
}

TEST_CASE("count", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto vec_size : test_arr_sizes) {
            auto haystack = iota_vector(vec_size);

            {
                int needle = 5;
                auto seq = std::count(poolstl::par_if<>(false), haystack.cbegin(), haystack.cend(), needle);
                auto par = std::count(poolstl::par.on(pool),    haystack.cbegin(), haystack.cend(), needle);
                REQUIRE(seq == par);
            }
            {
                auto pred = [&](auto x) { return x % 2 == 0; };
                auto seq = std::count_if(poolstl::par_if<>(false), haystack.cbegin(), haystack.cend(), pred);
                auto par = std::count_if(poolstl::par.on(pool),    haystack.cbegin(), haystack.cend(), pred);
                REQUIRE(seq == par);
            }
        }
    }
}

TEST_CASE("fill", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            std::vector<int> dest1(num_iters);
            std::vector<int> dest2(num_iters);

            std::fill(poolstl::par_if<>(false), dest1.begin(), dest1.end(), num_iters);
            std::fill(poolstl::par.on(pool),    dest2.begin(), dest2.end(), num_iters);

            REQUIRE(dest1 == dest2);
        }
    }
}

#if POOLSTL_HAVE_CXX17_LIB
TEST_CASE("fill_n", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        auto vec_size = *std::max_element(test_arr_sizes.cbegin(), test_arr_sizes.cend());
        for (auto num_iters : test_arr_sizes) {
            auto dest1 = iota_vector(vec_size);
            auto dest2 = iota_vector(vec_size);

            std::fill_n(poolstl::par_if<>(false), dest1.begin(), num_iters, num_iters);
            std::fill_n(poolstl::par.on(pool),    dest2.begin(), num_iters, num_iters);

            REQUIRE(dest1 == dest2);
        }
    }
}
#endif

TEST_CASE("find", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto vec_size : test_arr_sizes) {
            auto haystack = iota_vector(vec_size);

            for (int needle : {int(vec_size * 0.1),
                               int(vec_size * 0.1),
                               int(vec_size * 0.99),
                               vec_size,
                               vec_size + 100}) {
                {
                    // calls find_if
                    auto seq = std::find(poolstl::par_if<>(false), haystack.cbegin(), haystack.cend(), needle);
                    auto par = std::find(poolstl::par.on(pool),    haystack.cbegin(), haystack.cend(), needle);
                    REQUIRE(seq == par);
                }
                {
                    auto pred = [&](auto x) { return x < needle; };
                    // calls find_if
                    auto seq = std::find_if_not(poolstl::par_if<>(false), haystack.cbegin(), haystack.cend(), pred);
                    auto par = std::find_if_not(poolstl::par.on(pool),    haystack.cbegin(), haystack.cend(), pred);
                    REQUIRE(seq == par);
                }
            }
        }
    }
}

TEST_CASE("for_each", "[alg][algorithm]") {
    std::atomic<int> sum{0};
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v = iota_vector(num_iters);

            for (auto is_sequential : {true, false}) {
                sum = 0;
                auto f = [&](auto) { ++sum; };
                if (is_sequential) {
                    std::for_each(poolstl::par_if<>(false), v.cbegin(), v.cend(), f);
                } else {
                    std::for_each(poolstl::par.on(pool),    v.cbegin(), v.cend(), f);
                }
                REQUIRE(sum == num_iters);
            }
        }
    }
}

TEST_CASE("for_each_n", "[alg][algorithm]") {
    std::atomic<int> sum{0};
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        auto v = iota_vector(*std::max_element(test_arr_sizes.cbegin(), test_arr_sizes.cend()));

        for (auto num_iters : test_arr_sizes) {
            sum = 0;
            auto f = [&](auto) { ++sum; };
#if POOLSTL_HAVE_CXX17_LIB
            std::for_each_n(poolstl::par_if<>(false), v.cbegin(), num_iters, f);
            REQUIRE(sum == num_iters);
            sum = 0;
#endif
            std::for_each_n(poolstl::par.on(pool),    v.cbegin(), num_iters, f);
            REQUIRE(sum == num_iters);
        }
    }
}

TEST_CASE("sort", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            for (int scramble_type = 0; scramble_type <= 2; ++scramble_type) {
                auto source = iota_vector(num_iters);
                switch (scramble_type) {
                    case 0: std::reverse(source.begin(), source.end()); break;
                    case 1: scramble(source); break;
                    default: break;
                }
                std::vector<int> dest1(source);
                std::vector<int> dest2(source);

                std::sort(dest1.begin(), dest1.end());
                std::sort(poolstl::par_pool(pool), dest2.begin(), dest2.end());

                REQUIRE(dest1 == dest2);
            }
        }
    }
}

TEST_CASE("stable_sort", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            for (int scramble_type = 0; scramble_type <= 2; ++scramble_type) {
                auto source = iota_vector<stable_sort_element>(num_iters);
                for (auto& s: source) {
                    s.compared /= 3;
                    s.nc = (int)rng();
                }

                switch (scramble_type) {
                    case 0: std::reverse(source.begin(), source.end()); break;
                    case 1: scramble(source); break;
                    default: break;
                }
                std::vector<stable_sort_element> dest1(source);
                std::vector<stable_sort_element> dest2(source);

                std::stable_sort(dest1.begin(), dest1.end());
                std::stable_sort(poolstl::par_pool(pool), dest2.begin(), dest2.end());

                REQUIRE(dest1 == dest2);
            }
        }
    }
}

TEST_CASE("transform_1", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v = iota_vector(num_iters);
            std::vector<int> dest1(v.size());
            std::vector<int> dest2(v.size());

            auto unary_op = [&](auto x) { return 2*x; };

            auto par_res = std::transform(poolstl::par.on(pool),    v.cbegin(), v.cend(), dest1.begin(), unary_op);
#if POOLSTL_HAVE_CXX17_LIB
            auto seq_res = std::transform(poolstl::par_if<>(false), v.cbegin(), v.cend(), dest1.begin(), unary_op);
#else
            auto seq_res = poolstl::internal::cpp17::transform(v.cbegin(), v.cend(), dest1.begin(), unary_op);
#endif
            // test return value
            REQUIRE(par_res == seq_res);

            // test transformed values
            std::transform(poolstl::par_pool(pool), v.cbegin(), v.cend(), dest2.begin(), unary_op);
            REQUIRE(dest1 == dest2);
        }
    }
}

TEST_CASE("transform_2", "[alg][algorithm]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v1 = iota_vector(num_iters);
            auto v2 = iota_vector(num_iters, 1000);
            std::vector<int> dest1(v1.size());
            std::vector<int> dest2(v1.size());

            auto binary_op = [&](auto a, auto b) { return a + b; };

            auto par_res = std::transform(poolstl::par.on(pool),    v1.cbegin(), v1.cend(), v2.cbegin(), dest1.begin(), binary_op);
#if POOLSTL_HAVE_CXX17_LIB
            auto seq_res = std::transform(poolstl::par_if<>(false), v1.cbegin(), v1.cend(), v2.cbegin(), dest1.begin(), binary_op);
#else
            auto seq_res = poolstl::internal::cpp17::transform(v1.cbegin(), v1.cend(), v2.cbegin(), dest1.begin(), binary_op);
#endif
            // test return value
            REQUIRE(par_res == seq_res);

            // test transformed values
            std::transform(poolstl::par_pool(pool), v1.cbegin(), v1.cend(), v2.cbegin(), dest2.begin(), binary_op);
            REQUIRE(dest1 == dest2);
        }
    }
}

TEST_CASE("reduce", "[alg][numeric]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v = iota_vector(num_iters);

#if POOLSTL_HAVE_CXX17_LIB
            auto seq = std::reduce(poolstl::par_if<>(false), v.cbegin(), v.cend());
#else
            auto seq = poolstl::internal::cpp17::reduce(v.cbegin(), v.cend());
#endif
            auto par = std::reduce(poolstl::par.on(pool),    v.cbegin(), v.cend());
            REQUIRE(seq == par);
        }
    }
}

#if POOLSTL_HAVE_CXX17_LIB
TEST_CASE("transform_reduce_1", "[alg][numeric]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v = iota_vector(num_iters);

            auto doubler = [&](auto x) { return 2*x; };
            auto seq = std::transform_reduce(poolstl::par_if<>(false), v.cbegin(), v.cend(), 0, std::plus<>(), doubler);
            auto par = std::transform_reduce(poolstl::par.on(pool),    v.cbegin(), v.cend(), 0, std::plus<>(), doubler);
            REQUIRE(seq == par);
        }
    }
}

TEST_CASE("transform_reduce_2", "[alg][numeric]") {
    for (auto num_threads : test_thread_counts) {
        ttp::task_thread_pool pool(num_threads);

        for (auto num_iters : test_arr_sizes) {
            auto v1 = iota_vector(num_iters);
            auto v2 = iota_vector(num_iters);

            auto seq = std::transform_reduce(poolstl::par_if<>(false), v1.cbegin(), v1.cend(), v2.cbegin(), 0);
            auto par = std::transform_reduce(poolstl::par.on(pool),    v1.cbegin(), v1.cend(), v2.cbegin(), 0);
            REQUIRE(seq == par);
        }
    }
}
#endif

TEST_CASE("default_pool", "[execution]") {
    std::vector<int> v = {0, 1, 2, 3, 4, 5};
    auto sum = std::reduce(poolstl::par, v.cbegin(), v.cend());
    REQUIRE(sum == 15);
}

TEST_CASE("execution_policies", "[execution]") {
    ttp::task_thread_pool pool;
    std::vector<int> v = {0, 1, 2, 3, 4, 5};

    REQUIRE(1 == std::count(poolstl::par, v.cbegin(), v.cend(), 5));
    REQUIRE(1 == std::count(poolstl::par.on(pool), v.cbegin(), v.cend(), 5));
    REQUIRE(1 == std::count(poolstl::seq, v.cbegin(), v.cend(), 5));
    REQUIRE(1 == std::count(poolstl::par_if<>(false), v.cbegin(), v.cend(), 5));
    REQUIRE(1 == std::count(poolstl::par_if<>(true), v.cbegin(), v.cend(), 5));
    REQUIRE(1 == std::count(poolstl::par_if<>(false).on(pool), v.cbegin(), v.cend(), 5));
    REQUIRE(1 == std::count(poolstl::par_if<>(true).on(pool), v.cbegin(), v.cend(), 5));
}

TEST_CASE("iota_iter(use)", "[iterator]") {
    REQUIRE(15 == std::reduce(poolstl::par, iota_iter<int>(0), iota_iter<int>(6)));
    REQUIRE(1 == std::count(poolstl::par, iota_iter<int>(0), iota_iter<int>(6), 5));
    REQUIRE(1 == std::count(iota_iter<int>(0), iota_iter<int>(6), 5));
}

TEST_CASE("iota_iter(def)", "[iterator]") {
    // Test that iota_iter meets RandomAccessIterator requirements.
    // See https://en.cppreference.com/w/cpp/iterator/random_access_iterator

    long a_init = 5;
    long b_init = 10;
    iota_iter<long> a(a_init);
    iota_iter<long> b(b_init);
    auto n = std::distance(a, b);
    REQUIRE(n == 5);

    // (a += n) is equal to b.
    REQUIRE((a += n) == b);
    a = a_init; b = b_init;

    // std::addressof(a += n) is equal to std::addressof(a). [1]
    REQUIRE(std::addressof(a += n) == std::addressof(a));

    // (a + n) is equal to (a += n).
    {
        auto lhs = (a + n);
        REQUIRE(lhs == (a += n));
        a = a_init; b = b_init;
    }

    // (a + n) is equal to (n + a).
    REQUIRE((a + n) == (n + a));

    // For any two positive integers x and y, if a + (x + y) is valid, then a + (x + y) is equal to (a + x) + y.
    int x = 12, y = 55;
    REQUIRE((a + (x + y)) == ((a + x) + y));

    // a + 0 is equal to a.
    REQUIRE((a + 0) == a);

    // If (a + (n - 1)) is valid, then --b is equal to (a + (n - 1)).
    REQUIRE(--b == (a + (n - 1)));
    a = a_init; b = b_init;

    // (b += -n) and (b -= n) are both equal to a.
    REQUIRE((b += -n) == a);
    a = a_init; b = b_init;

    REQUIRE((b -= n) == a);
    a = a_init; b = b_init;

    // std::addressof(b -= n) is equal to std::addressof(b). [1]
    REQUIRE(std::addressof(b -= n) == std::addressof(b));
    a = a_init; b = b_init;

    // (b - n) is equal to (b -= n).
    {
        auto lhs = (b - n);
        REQUIRE(lhs == (b -= n));
        a = a_init; b = b_init;
    }

    // If b is dereferenceable, then a[n] is valid and is equal to *b.
    REQUIRE(a[n] == *b);

    // bool(a <= b) is true.
    REQUIRE(bool(a <= b));


    // exercise the other methods
    REQUIRE(a == a);
    REQUIRE(a != b);
    REQUIRE(a < b);
    REQUIRE(b > a);
    REQUIRE(b >= a);

    REQUIRE(std::addressof(++a) == std::addressof(a));
    REQUIRE(std::addressof(--a) == std::addressof(a));

    {
        auto lhs = a;
        REQUIRE(lhs == a++);
    }

    {
        auto lhs = (a + 1);
        REQUIRE(lhs == ++a);
    }

    {
        auto lhs = a;
        REQUIRE(lhs == a--);
    }

    {
        auto lhs = (a - 1);
        REQUIRE(lhs == --a);
    }
    
    // default constructible
    iota_iter<long> c;
    REQUIRE(*c == 0);
}

std::mt19937 rng{1};

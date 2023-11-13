// Copyright (C) 2023 Adam Lugowski. All rights reserved.
// Use of this source code is governed by:
// the BSD 2-clause license, the MIT license, or at your choosing the BSL-1.0 license found in the LICENSE.*.txt files.
// SPDX-License-Identifier: BSD-2-Clause OR MIT OR BSL-1.0


#include <array>
#include <numeric>

constexpr std::array<int, 6> test_thread_counts = {0, 1, 2, 3, 5, 10};
constexpr std::array<int, 14> test_arr_sizes = {0, 1, 2, 3, 4, 8, 9, 10, 11, 20, 23, 77, 101};

template<typename T>
struct i_iter {
    T value;

    explicit i_iter(T v) : value(v) {}

    T operator*() const { return value; }

    T operator++() { return ++value; }

    template<typename U>
    i_iter operator+(const U &other) { return i_iter<T>(value + other); }

    T operator-(const i_iter<T> &other) const { return value - other.value; }

    bool operator<(const i_iter<T> &other) const { return value < other.value; }
    bool operator==(const i_iter<T> &other) const { return value == other.value; }
    bool operator!=(const i_iter<T> &other) const { return value != other.value; }
};

template <typename T=int>
std::vector<T> iota_vector(size_t size, T init=0) {
    std::vector<T> ret(size);
    std::iota(ret.begin(), ret.end(), init);
    return ret;
}

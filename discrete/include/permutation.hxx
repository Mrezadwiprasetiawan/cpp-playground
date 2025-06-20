/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <concepts>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace Discrete {
template <typename T> requires(std::integral<T> && !std::is_same_v<T, bool>) class Permutation {
  static std::vector<T> caches;

 public:
  explicit Permutation() {}
  explicit Permutation(T init_cache, k = 0) { Permutation::calc(init_cache, k); }

  // Hitung nP_k = n! / (n-k)!
  static T calc(T n, T k) {
    if (n < k) throw std::runtime_error("n must be >= k");
    if (k == 0) return 1;
    return n * calc(n - 1, k - 1);
  }

  // All sources permutation with the length k
  template <typename U> std::vector<std::vector<U>> permute(const std::vector<U>& sources, T k = 0) {
    if (k < 0) throw std::runtime_error("k must be non-negative");
    T n = static_cast<T>(sources.size());
    if (k == 0) return {{}};
    if (n < k) throw std::runtime_error("sources.size() must be >= k");

    auto total = calc(n, k);
    std::vector<std::vector<U>> res;
    res.reserve(total);

    std::vector<U> curr;
    curr.reserve(k);

    std::vector<bool> used(static_cast<std::size_t>(n), false);
    // Depth First Search
    std::function<void()> dfs = [&]() {
      if (static_cast<T>(curr.size()) == k) {
        res.push_back(curr);
        return;
      }
      for (size_t i = 0; i < n; ++i) {
        if (used[i]) continue;
        used[i] = true;
        curr.push_back(sources[i]);
        dfs();
        used[i] = false;
      }
    };

    dfs();
    return res;
  }
};

template <typename T> std::enable_if_t<std::is_integral_v<T>, T> (*factorial)(T n) = Permutation<T>::calc(n);
}  // namespace Discrete
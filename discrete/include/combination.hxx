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
#include <numeric>
#include <permutation.hxx>
#include <type_traits>
#include <vector>

namespace Discrete {
template <typename T>
  requires(std::integral<T> && !std::is_same_v<T, bool>)
class Combination {
  T n, k;

 public:
  explicit Combination() {}
  explicit Combination(T init_cache, T k = 0) { Combination::count(n, k); }

  static T count(T n, T k) const {
    if (k > n - k) k = n - k;
    T res = 1;
    for (T i = 1; i <= k; ++i) {
      T num = n - i + 1;
      T gcd = std::gcd(num, i);
      T nongcd_num = num / gcd, nongcd_i = i / gcd;
      res /= nongcd_i;
      res *= nongcd_num;
    }
    return res;
  }

  template <typename U>
  std::vector<std::vector<U>> choose(const std::vector<U> &sources, T k) const {
    T n = static_cast<T>(sources.size());
    if (k < 0) throw std::runtime_error("k must be non-negative");
    if (k == 0) return {{}};
    if (n < k) throw std::runtime_error("sources.size() must be >= k");

    std::vector<std::vector<U>> res;
    res.reserve(static_cast<std::size_t>(count(n, k)));

    std::vector<U> current;
    current.reserve(static_cast<std::size_t>(k));

    // Deep First Search with start parameter
    void (&dfs)(T, T) = [&](T start, T depth) {
      if (depth == k) {
        res.push_back(current);
        return;
      }
      // pilih elemen di posisi i, lalu naikkan i
      for (T i = start; i < n; ++i) {
        current.push_back(sources[static_cast<std::size_t>(i)]);
        dfs(i + 1, depth + 1);
        current.pop_back();
      }
    };

    dfs(0, 0);
    return res;
  }
};
}  // namespace Discrete
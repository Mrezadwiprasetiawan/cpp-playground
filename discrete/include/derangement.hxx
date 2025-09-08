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

// Derangement.hpp – compact hybrid derangement calculator (integer‑only except cutoff logic)
#pragma once

#include <cmath>
#include <concepts>
#include <functional>
#include <type_traits>
#include <vector>

namespace Discrete {

template <typename T>
requires(std::integral<T> && !std::is_same_v<T, bool>) class Derangement {
  static constexpr double log_phi = 0.48121182505960347;
  static constexpr T      magic_number =
      static_cast<T>(std::log((3.0 * std::sqrt(5.0)) / 5.0) /
                     log_phi);  // depends on the machine architecture, but this just approximation for 1 operation building recursive caches of the derangement
  static inline std::vector<T> cache = {1, 0};  // !0 = 1, !1 = 0

  // Compute cutoff based on k_min ≈ log_phi((3√5 / 5) * n)
  static T min_k_pie_faster(T n) {
    // We keep the 3/5 * sqrt(5) multiplier exactly
    // and use log base φ to stay faithful to the derivation
    // phi = (1 + sqrt(5)) / 2 ≈ 1.61803
    return static_cast<T>(n / log_phi + magic_number);
  }

  static T pie_calc(T n) {
    T k_max = min_k_pie_faster(n);
    if (k_max > n) k_max = n;

    T   term = 1;
    T   sum  = 1;
    int sign = -1;

    for (T i = 1; i <= k_max; ++i) {
      term *= (n - i + 1);  // P(n,i)
      sum  += sign * term;
      sign  = -sign;
    }

    if ((k_max == n) && (n & 1)) sum = -sum;
    return sum;
  }

  static T recursive_calc(T n) {
    if (n < static_cast<T>(cache.size())) return cache[n];
    cache.resize(n + 1);
    for (T i = static_cast<T>(cache.size() - 1); i <= n; ++i) cache[i] = (i - 1) * (cache[i - 1] + cache[i - 2]);
    return cache[n];
  }

 public:
  static T calc(T n) { return (n > min_k_pie_faster(n)) ? pie_calc(n) : recursive_calc(n); }

  template <typename U>
  std::vector<std::vector<U>> derange(const std::vector<U> &src) {
    std::vector<std::vector<U>> result;
    std::vector<U>              current(src.size());
    std::vector<bool>           used(src.size(), false);

    std::function<void(size_t)> dfs = [&](size_t i) {
      if (i == src.size()) {
        result.push_back(current);
        return;
      }
      for (size_t j = 0; j < src.size(); ++j) {
        if (used[j] || j == i) continue;
        used[j]    = true;
        current[i] = src[j];
        dfs(i + 1);
        used[j] = false;
      }
    };

    dfs(0);

    return result;
  }
};

}  // namespace Discrete

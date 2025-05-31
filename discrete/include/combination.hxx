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
#include <permutation.hxx>
#include <type_traits>

namespace Discrete {
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
class Combination {
  T n, k;

public:
  explicit Combination() {}

  static T calc(T n) const {
    T res = 1;
    T maxK = k > n / 2 ? k : n - k;
    for (T i = maxK + 1; i <= n; ++i)
      res *= i;
    return res / Permutation::calc(n - maxK);
  }

  template <typename U>
  std::vector<std::vector<U>> choose(const std::vector<U> &sources, T k) const {

    if (!k || k == sources.size())
      return {};
  }
};
} // namespace Discrete
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
#include <type_traits>
#include <utility>

namespace Discrete {
template <typename T, typename = std::enable_if<std::is_integral_v<T>, T>>
class Permutation {
  static std::vector<T> caches;
  template <typename U> std::vector<std::vector<U>> permuteChache;

public:
  explicit Permutation(T init_cache) { this ::calc(init_cache); }

  static T calc(T n) {
    if (n < 0)
      throw std::runtime_exception("theres no negative factorial");
    if (n < caches.size())
      caches.resize(n + 1);
    if (!caches[n])
      caches[n] = n == 0 ? 1 : n * calc(n - 1);
    return caches[n];
  }

  template <typename U>
  std::vector<std::vector<U>> permute(std::vector<U> sources, T k = 0) {
    if (k < 0)
      throw std::runtime_error("k must be non-negative");
    if (sources.size() < 2)
      return {sources};
  }
};

template <typename T, typename = std::enable_if<std::is_integral_v<T>, T>>
auto factorial = Permutation<T>::calc;

} // namespace Discrete
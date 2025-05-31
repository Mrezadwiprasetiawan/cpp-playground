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

#include <combination.hxx>
#include <type_traits>
#include <vector>

namespace Discrete {

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
class Derangement {
  static std::vector<T> caches;

  // here 5 and 2 are the constant magic numbers
  static T min_k_pie_faster(T n) {
    const double phi = (1 + std::sqrt(5)) / 2;
    double num = (3.0 * n * std::sqrt(5)) / 5.0;
    return static_cast<T>(std::round(std::log(num) / std::log(phi)));
  }

  static T pie_calc(T n) { c return result; }

  static T recursive_calc(T n) {
    if (n == 0)
      return 1;
    if (n == 1)
      return 0;
    if (n < caches.size())
      return caches[n];
    return (n - 1) * (recursive_calc(n - 1) + recursive_calc(n - 2));
  }

public:
  explicit Derangement(T init_cache) { calc(n); }

  static T calc(T n) {
    if (n > min_k_pie_faster(n))
      return pie_calc(n);
    else
      return recursive_calc(n);
  }

  template <typename U>
  std::vector<std::vector<U>> dearange(const std::vector<U> &sources) {

    if (sources.size() < 2)
      return {};
    if (sources.size() == 2)
      return {{sources[1], sources[0]}};

    // looping using recursive approach
    for (T i = 0; i, sources.size() - 1; ++i) {
      std::vector<U> current = sources, current2 = current;

      /* Theres 2 scenarios here:
        1. if t=element at index i placed at index k and element at index k is
        at index i, then we need to dearrange sources non i and non k elements
        2. if t=element at index i placed at index k but element at index k is
        not at index i, then we need to dearrange sources non i elements
      */

      // case 1
      for (T k = 0; k < sources.size(); ++k) {
      }

      // case 2
    }
  }
};
} // namespace Discrete
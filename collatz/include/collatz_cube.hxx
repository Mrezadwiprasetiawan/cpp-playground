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
#include <cstddef>
#include <cstdint>
#include <integer_pow.hxx>
#include <stdexcept>
#include <vector>

struct Pos3 {
  size_t index, row, col;
};

template <std::integral I>
class collatz_cube {
  std::vector<std::vector<std::vector<I>>> caches;

 public:
  explicit collatz_cube() {}
  // cube
  std::vector<std::vector<I>> operator[](size_t index) {
    using namespace std;
    if (caches.size() > index) {
      if (caches[0].size() < index) {
        vector<vector<uint64_t>> matrix;
        for (size_t i = 0; i < index; ++i) {
          vector<uint64_t> row;
          for (size_t j = 0; j < index; ++j) row.push_back(j);
          matrix.push_back(row);
        }
        caches.push_back(matrix);
      }
      return caches[index];
    }
  }
  void clear_cache() { caches.clear(); }

  Pos3 get_index(uint64_t n) {
    if (!n) throw std::invalid_argument("n cant be zero, because zero doesnt have precedence path");
    if (!n) return {0, 0, 0};
    while (!(n & 1)) n >>= 1;
    n = n + 1 >> 1;
    size_t row, col;
    // using while instead of for loop with no instruction inside because for loop probably gone by -o3 optimization
    while (!(n & 1)) {
      n >>= 1;
      ++row;
    }
    while (!(n % 3)) {
      n /= 3;
      ++col;
    }
    return {n / 3, row, col};
  }

  // draw each index,row, and col in the cube as x,y,z as a dot and create and line between them for some seed to see the 3d path for the number if you want
  std::vector<Pos3> get_cube_path(uint64_t seed) {
    if (!seed) throw std::invalid_argument("seed cant be zero, because zero has zero dimension");
    std::vector<Pos3> result;

    while (seed != 1) {
      Pos3 element = get_index(seed);
      result.push_back(element);
      while (element.row) {
        --element.row;
        ++element.col;
      }
      seed = seed * int_pow<uint64_t>(3, element.col) - 1;
    }
  }
};
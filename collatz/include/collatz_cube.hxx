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
class Collatz_cube {
  std::vector<std::vector<std::vector<I>>> caches;

 public:
  explicit Collatz_cube() {}
  // cube
  std::vector<std::vector<I>> operator[](size_t index) noexcept {
    using namespace std;
    const size_t upperbound        = index + 1;
    const size_t currentUpperbound = caches.size();
    if (index > caches.size() - 1) {
      if (caches.size() == 0) {
        for (size_t i = 0; i < upperbound; ++i) {
          vector<vector<uint64_t>> matrix;
          vector<uint64_t>         firstRow;
          firstRow.push_back(i & 1 ? i * 3 + 2 : i * 3 + 1);
          for (size_t k = 1; k < upperbound; ++k) firstRow.push_back(firstRow[k - 1] * 3);
          matrix.push_back(firstRow);
          for (size_t j = 1; j < upperbound; ++j) {
            vector<uint64_t> row;
            row.push_back(matrix[j - 1][0] << 1);
            for (size_t k = 1; k < upperbound; ++k) row.push_back(row[k - 1] * 3);
          }
        }
      } else {
        for (size_t i = 0; i < currentUpperbound; ++i) {
          for (size_t j = 0; j < currentUpperbound; ++j)
            for (size_t k = caches.size(); k < upperbound; ++k) caches[i][j].push_back(caches[i][j][k - 1] * 3);
          for (size_t i = caches.size(); i < upperbound; ++i) {
            vector<vector<I>> newMatrix;
            for (size_t j = currentUpperbound; j < upperbound; ++j) {
              vector<I> newElement;
              newMatrix.push_back(newElement);
            }
            caches.push_back(newMatrix);
          }
        }
      }
    }
    return caches[index];
  }

  void clear_cache() noexcept { caches.clear(); }

  Pos3 get_index(uint64_t n) {
    if (!n) throw std::invalid_argument("n cant be zero, because zero has no precedence");
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
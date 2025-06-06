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

#include <algorithm>
#include <big_int.hxx>
#include <string>

template <typename T, typename = std::enable_if<std::is_integral_v<T> or std::is_same_v<T, Big_int>, T>>
std::string to_string(T val, int base) {
  string res;
  while (val) {
    res += (val % base) + '0';
    val /= base;
  }
  std::reverse(res.begin(), res.end());
  return res;
}
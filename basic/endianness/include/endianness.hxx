/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan

  This project contains various experiments and explorations in C++,
  including topics such as number systems, neural networks, and 
  visualizations of prime number patterns.

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


#include <cstdint>

enum Endianness { BIG, LITTLE };

inline bool is_little_endian() {
  uint16_t testdata = 0x0100;
  uint8_t *testarr = reinterpret_cast<uint8_t *>(&testdata);
  if (testarr[0] & 1) return false;
  return true;
}

inline bool is_big_endian() { return !is_little_endian(); }

const Endianness CURRENT_ENDIANNESS =
    is_little_endian() ? Endianness::LITTLE : Endianness::BIG;

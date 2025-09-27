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

template <std::integral I>
I int_pow(I base, I exp) {
  if constexpr (I(-1) < 0) if (exp < 0) return 0;

  if (!exp) return 1;

  int bitcount = 0;

#if defined(__clang__) || defined(__GNUC__)
  if constexpr (sizeof(I) <= 4) {
    bitcount = __builtin_popcount(static_cast<std::uint32_t>(exp));
  } else {
    bitcount = __builtin_popcountll(static_cast<std::uint64_t>(exp));
  }
#elif defined(_MSC_VER)
  if constexpr (sizeof(I) <= 4) {
    bitcount = __popcnt(static_cast<unsigned>(exp));
  } else {
    bitcount = static_cast<int>(__popcnt64(static_cast<unsigned long long>(exp)));
  }
#else
  { // fallback bit-parallel
    if constexpr (sizeof(I) <= 4) {
      std::uint32_t v = static_cast<std::uint32_t>(exp);
      v = v - ((v >> 1) & 0x55555555u);
      v = (v & 0x33333333u) + ((v >> 2) & 0x33333333u);
      v = (v + (v >> 4)) & 0x0F0F0F0Fu;
      bitcount = static_cast<int>((v * 0x01010101u) >> 24);
    } else {
      std::uint64_t v = static_cast<std::uint64_t>(exp);
      v = v - ((v >> 1) & 0x5555555555555555ULL);
      v = (v & 0x3333333333333333ULL) + ((v >> 2) & 0x3333333333333333ULL);
      v = (v + (v >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
      bitcount = static_cast<int>((v * 0x0101010101010101ULL) >> 56);
    }
  }
#endif

  if (bitcount == 1) {
    int bitpos = 0;
    auto cp = static_cast<std::make_unsigned_t<I>>(exp);
    while (cp > 1) { cp >>= 1; ++bitpos; }
    I res = base;
    for (int i = 0; i < bitpos; i++) res *= res;
    return res;
  }

  if (exp & 1) return base * int_pow(base, exp - 1);
  return int_pow(base * base, exp >> 1);
}
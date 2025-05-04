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

#include <cstdint>

// TYPE
template <bool b, typename T>
struct is_type {
  static const bool val = b;
};

template <typename T>
struct is_type<true, T> {
  static const bool val = true;
  using type = T;
};

template <bool b, typename T>
using is_type_t = typename is_type<b, T>::type;

template <typename A, typename B>
bool is_same = false;

template <typename A>
bool is_same<A, A> = true;

// floating point
template <typename T>
struct is_fp : is_type<false, T> {};

#define _IS_FLOAT_TEMPLATE_DECL(type) \
  template <>                         \
  struct is_fp<type> : is_type<true, type> {};

_IS_FLOAT_TEMPLATE_DECL(float);
_IS_FLOAT_TEMPLATE_DECL(double);
#undef _IS_FLOAT_TEMPLATE_DECL

// Integer
template <typename T>
struct is_int : is_type<false, T> {};

#define _IS_INTEGER_TEMPLATE_DECL(type) \
  template <>                           \
  struct is_int<type> : is_type<true, type> {}

_IS_INTEGER_TEMPLATE_DECL(int8_t);
_IS_INTEGER_TEMPLATE_DECL(uint8_t);
_IS_INTEGER_TEMPLATE_DECL(int16_t);
_IS_INTEGER_TEMPLATE_DECL(uint16_t);
_IS_INTEGER_TEMPLATE_DECL(int32_t);
_IS_INTEGER_TEMPLATE_DECL(uint32_t);
_IS_INTEGER_TEMPLATE_DECL(int64_t);
_IS_INTEGER_TEMPLATE_DECL(uint64_t);
#undef _IS_INTEGER_TEMPLATE_DECL

// LOGIC
template <bool b, typename T, typename F>
struct ifel {
  using type = F;
};

template <typename T, typename F>
struct ifel<true, T, F> {
  using type = T;
};

template <bool b, typename T, typename F>
using ifel_t = typename ifel<b, T, F>::type;

template <typename Trait, typename F>
using ifel_trait_t = ifel_t<Trait::val, typename Trait::type, F>;

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

#include <cassert>
#include <cmath>
#include <concepts>
#include <initializer_list>

namespace Linear {

template <typename T, int N>
requires(std::integral<T> || std::floating_point<T>) class Vec {
 private:
  T val[N];

 public:
  Vec() : val() {}

  Vec(std::initializer_list<T> list) {
    assert(list.size() == N);
    auto it = list.begin();
    for (int i = 0; i < N; ++i, ++it) val[i] = *it;
  }

  Vec(T (&arr)[N]) {
    for (int i = 0; i < N; ++i) val[i] = arr[i];
  }

#define VEC_BASE_OPERATOR(op)                                             \
  template <typename U>                                                   \
  Vec operator op(const Vec<U, N> &vn) const {                            \
    Vec<T, N> res;                                                        \
    for (int i = 0; i < N; ++i) res[i] = val[i] op static_cast<T>(vn[i]); \
    return res;                                                           \
  }

  VEC_BASE_OPERATOR(+)
  VEC_BASE_OPERATOR(-)
  VEC_BASE_OPERATOR(*)
  VEC_BASE_OPERATOR(/)
#undef VEC_BASE_OPERATOR

#define VEC_OV_ASSIGNMENT(op)                \
  template <typename U>                      \
  Vec &operator op##=(const Vec<U, N> &vn) { \
    return *this = *this op vn;              \
  }

  VEC_OV_ASSIGNMENT(+);
  VEC_OV_ASSIGNMENT(-);
  VEC_OV_ASSIGNMENT(*);
  VEC_OV_ASSIGNMENT(/);
#undef VEC_OV_ASSIGNMENT

  // di dalam class Vec
  auto w() const requires(N >= 4) { return val[3]; }

  auto x() const requires(N >= 1) { return val[0]; }
  auto y() const requires(N >= 2) { return val[1]; }
  auto z() const requires(N >= 3) { return val[2]; }

  T       &operator[](std::size_t i) { return val[i]; }
  const T &operator[](std::size_t i) const { return val[i]; }
  T       *data() { return val; }
};

template <typename T, int N>
requires(std::floating_point<T>) Vec<T, N> normalize(Vec<T, N> target) {
  T length = 0;
  for (int i = 0; i < N; ++i) length += target[i] * target[i];
  length = std::sqrt(length);
  for (int i = 0; i < N; ++i) target[i] = target[i] / length;
  return target;
}

template <typename T, int N>
requires(std::floating_point<T>) T dot(const Vec<T, N> &a, const Vec<T, N> &b) {
  T res = 0;
  for (int i = 0; i < N; ++i) res += a[i] * b[i];
  return res;
}

template <typename T>
requires(std::floating_point<T>) Vec<T, 3> cross(const Vec<T, 3> &a, const Vec<T, 3> &b) {
  return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}

template <typename T>
using Vec3 = Vec<T, 3>;
template <typename T>
using Vec4  = Vec<T, 4>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;

}  // namespace Linear

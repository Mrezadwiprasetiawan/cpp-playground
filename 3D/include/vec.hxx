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
#include <custom_trait.hxx>
#include <initializer_list>

namespace l3d {

template <typename T, int N>
class Vec {
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

#define VEC_BASE_OPERATOR(op)                             \
  template <typename U>                                   \
  Vec operator op(const Vec<U, N> &vn) const {            \
    Vec<T, N> res;                                        \
    for (int i = 0; i < N; ++i) res[i] = val[i] op vn[i]; \
    return res;                                           \
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

#define GETTER_XYZ(type, index)         \
  template <typename U = T>             \
  is_type_t<(N >= 3), U> type() const { \
    return val[index];                  \
  }

  GETTER_XYZ(x, 0);
  GETTER_XYZ(y, 1);
  GETTER_XYZ(z, 2);
#undef GETTER_XYZ
  template <typename U = T>
  is_type_t<(N >= 4), U> w() {
    return val[3];
  }

  T &operator[](std::size_t i) { return val[i]; }
  const T &operator[](std::size_t i) const { return val[i]; }
  T *data() { return val; }
};

template <typename T, int N, typename = ifel_trait_t<is_fp<T>, float>>
Vec<T, N> normalize(Vec<T, N> target) {
  T length = 0;
  for (int i = 0; i < N; ++i) length += target[i] * target[i];
  length = sqrt(length);
  for (int i = 0; i < N; ++i) target[i] = target[i] / length;
  return target;
}

template <typename T, int N, typename = ifel_trait_t<is_fp<T>, float>>
T dot(const Vec<T, N> &a, const Vec<T, N> &b) {
  T res = 0;
  for (int i = 0; i < N; ++i) res += a[i] * b[i];
  return res;
}

template <typename T, typename = ifel_trait_t<is_fp<T>, float>>
Vec<T, 3> cross(const Vec<T, 3> &a, const Vec<T, 3> &b) {
  return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
          a[0] * b[1] - a[1] * b[0]};
}

template <typename T>
using Vec3 = Vec<T, 3>;
template <typename T>
using Vec4 = Vec<T, 4>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;

}  // namespace l3d

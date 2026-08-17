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
#include <iostream>
#include <string>

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

  // converting copy constructor, agar Vec<T,N> bisa dicopy dari Vec<U,N> dengan tipe elemen berbeda
  template <typename U>
  Vec(const Vec<U, N> &other) {
    for (int i = 0; i < N; ++i) val[i] = static_cast<T>(other[i]);
  }

  // converting copy assignment, sama seperti di atas tapi untuk operator=
  template <typename U>
  Vec &operator=(const Vec<U, N> &other) {
    for (int i = 0; i < N; ++i) val[i] = static_cast<T>(other[i]);
    return *this;
  }

  // ==== operasi in-place, method ini langsung memodifikasi object saat ini (void, tanpa return) ====
  // dipakai sebagai basis untuk operator biasa (yang mengembalikan objek baru hasil copy)
  // dan operator penugasan (+=, -=, dst) supaya tidak ada copy yang sia-sia

  template <typename U>
  void add(const Vec<U, N> &vn) {
    for (int i = 0; i < N; ++i) val[i] += static_cast<T>(vn[i]);
  }

  template <typename U>
  void sub(const Vec<U, N> &vn) {
    for (int i = 0; i < N; ++i) val[i] -= static_cast<T>(vn[i]);
  }

  template <typename U>
  void mul(const Vec<U, N> &vn) {
    for (int i = 0; i < N; ++i) val[i] *= static_cast<T>(vn[i]);
  }

  template <typename U>
  void div(const Vec<U, N> &vn) {
    for (int i = 0; i < N; ++i) val[i] /= static_cast<T>(vn[i]);
  }

  // ==== operator biasa, hasilnya selalu object baru (copy dulu baru dipanggil in-place-nya) ====

  template <typename U>
  Vec operator+(const Vec<U, N> &vn) const {
    Vec res(*this);
    res.add(vn);
    return res;
  }

  template <typename U>
  Vec operator-(const Vec<U, N> &vn) const {
    Vec res(*this);
    res.sub(vn);
    return res;
  }

  template <typename U>
  Vec operator*(const Vec<U, N> &vn) const {
    Vec res(*this);
    res.mul(vn);
    return res;
  }

  template <typename U>
  Vec operator/(const Vec<U, N> &vn) const {
    Vec res(*this);
    res.div(vn);
    return res;
  }

  // ==== operator penugasan, langsung pakai method in-place di atas, tanpa copy dulu ====

  template <typename U>
  Vec &operator+=(const Vec<U, N> &vn) {
    add(vn);
    return *this;
  }

  template <typename U>
  Vec &operator-=(const Vec<U, N> &vn) {
    sub(vn);
    return *this;
  }

  template <typename U>
  Vec &operator*=(const Vec<U, N> &vn) {
    mul(vn);
    return *this;
  }

  template <typename U>
  Vec &operator/=(const Vec<U, N> &vn) {
    div(vn);
    return *this;
  }

  // di dalam class Vec
  auto w() const requires(N >= 4) { return val[3]; }

  auto x() const requires(N >= 1) { return val[0]; }
  auto y() const requires(N >= 2) { return val[1]; }
  auto z() const requires(N >= 3) { return val[2]; }

  T       &operator[](std::size_t i) { return val[i]; }
  const T &operator[](std::size_t i) const { return val[i]; }
  T       *data() { return val; }

  void debug(const std::string &name) {
    std::cout << name << " = [\t";
    for (int i = 0; i < N; ++i) std::cout << val[i] << "\t";
    std::cout << "]\n";
  }
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
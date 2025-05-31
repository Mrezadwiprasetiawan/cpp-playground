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

#include "vec.hxx"
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <initializer_list>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace Linear {

// example usage Mat<double,4> Matrix 4 * 4 with double element type
template <typename T, int N,
          typename = std::enable_if_t<std::is_floating_point_v<T>, T>>
class Mat {
private:
  T vals[N * N];

public:
  Mat() : vals() {}

  Mat(T (&v)[N * N]) { set_elements(v); }

  Mat(const std::initializer_list<T> &v) { set_elements(v); }

  Mat(T (&v)[N][N]) {
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j)
        vals[N * i + j] = v[i][j];
  }

  void to_array(T (&arr)[N * N]) const {
    for (int i = 0; i < N * N; ++i)
      arr[i] = vals[i];
  }

  std::vector<T> to_vector() const {
    return std::vector<T>(vals, vals + N * N);
  }

  void set_elements(T (&v)[N * N]) {
    for (int i = 0; i < N * N; ++i)
      vals[i] = v[i];
  }

  void set_elements(const std::vector<T> &v) {
    assert(v.size() == N * N);
    for (int i = 0; i < N * N; ++i)
      vals[i] = v[i];
  }

  void set_elements(const std::initializer_list<T> &v) {
    assert(v.size() == N * N);
    auto it = v.begin();
    // tricky menambah i bersamaan dengan menambah iterator it
    for (int i = 0; i < N * N; ++i, ++it)
      vals[i] = *it;
  }

  void set_identity() {
    for (int row = 0; row < N; ++row)
      for (int col = 0; col < N; ++col)
        if (row == col)
          vals[row * N + col] = 1.0;
        else
          vals[row * N + col] = 0;
  }

  void set_element(size_t i, T valsue) { vals[i] = valsue; }

  template <typename U> Mat operator*(U fp) const {
    T vals_cp[N * N];
    for (int i = 0; i < N * N; ++i)
      vals_cp[i] = vals[i] * fp;
    return Mat(vals_cp);
  }

  Vec<T, N> operator*(const Vec<T, N> &vn) const {
    Vec<T, N> res;
    for (int row = 0; row < N; ++row)
      for (int col = 0; col < N; ++col)
        res[row] += vals[row * N + col] * vn[col];
    return res;
  }

  template <typename U> Mat operator*(const Mat<U, N> &m) const {
    T vals_res[N * N]{};
    // k ini faktor untuk ngurusin perkaliannya
    // sedangkan row dan col untuk indeks hasil akhirnya
    for (int row = 0; row < N; ++row)
      for (int col = 0; col < N; ++col)
        for (int k = 0; k < N; ++k)
          vals_res[row * N + col] += vals[row * N + k] * m[k][col];

    return Mat(vals_res);
  }

  template <typename U> Mat operator+(const Mat<U, N> &m) const {
    T vals_res[N * N]{};
    for (int i = 0; i < N * N; ++i)
      vals_res[i] = vals[i] + m[i / N][i % N];
    return Mat(vals_res);
  }

  template <typename U> Mat operator-(const Mat<U, N> &m) const {
    T vals_res[N * N]{};
    for (int i = 0; i < N * N; ++i)
      vals_res[i] = vals[i] - m[i / N][i % N];
    return Mat(vals_res);
  }

  // overload juga penugasannya agar lebih mudah
#define OV_ASSIGNMENT_OP(op)                                                   \
  template <typename U> Mat &operator op##=(const Mat<U, N> &m) {              \
    return *this = *this op m;                                                 \
  }

  /* nambah ; sebenernya ga perlu tapi karena vim indentnya bakal ga sejajar
   * lagi jadi ditambahin ;
   */
  OV_ASSIGNMENT_OP(*);
  OV_ASSIGNMENT_OP(/);
  OV_ASSIGNMENT_OP(+);
  OV_ASSIGNMENT_OP(-);
#undef OV_ASSIGNMENT_OP

  // row
  Vec<T, N> operator[](size_t index) const {
    T arr[N];
    for (int i = 0; i < N; ++i)
      arr[i] = vals[index * N + i];
    return Vec<T, N>(arr);
  }

  // eliminasi gauss
  T determinant() const {
    T det = 1;
    T tmpvals[N * N];
    to_array(tmpvals);
    for (int i = 0; i < N; ++i) {
      // cari baris dengan elemen terbesar di kolom i (pivoting)
      int maxRow = i;
      for (int j = i + 1; j < N; ++j)
        if (std::abs(tmpvals[j * N + i]) > std::abs(tmpvals[maxRow * N + i]))
          maxRow = j;
      // tukar baris i dengan baris maxRow jika perlu
      if (i != maxRow) {
        for (int k = 0; k < N; ++k)
          std::swap(tmpvals[i * N + k], tmpvals[maxRow * N + k]);
        det = -det; // Perubahan tanda jika ada pertukaran baris
      }
      // jika elemen diagonalnya 0, matriks tidak invertible, det = 0
      if (tmpvals[i * N + i] == 0)
        return 0;
      // eliminasi Gauss untuk baris di bawahnya
      for (int j = i + 1; j < N; ++j) {
        T factor = tmpvals[j * N + i] / tmpvals[i * N + i];
        for (int k = i; k < N; ++k)
          tmpvals[j * N + k] -= factor * tmpvals[i * N + k];
      }
      // Kalikan elemen diagonal untuk determinan
      det *= tmpvals[i * N + i];
    }
    return det;
  }

  Mat transpose() const {
    // tukar baris menjadi kolom dan kolom menjadi baris
    T tmpvals[N * N];
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j)
        tmpvals[i * N + j] = vals[j * N + i];
    return Mat(tmpvals);
  }

  Mat inverse() const {
    T res[N][N], tmp[N][N];
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j) {
        tmp[i][j] = vals[i * N + j];
        res[i][j] = (i == j ? 1 : 0);
      }

    for (int i = 0; i < N; ++i) {
      int pivot = i;
      for (int j = i + 1; j < N; ++j)
        if (std::abs(tmp[j][i]) > std::abs(tmp[pivot][i]))
          pivot = j;
      if (tmp[pivot][i] == 0)
        throw std::runtime_error("Singular matrix");

      for (int k = 0; k < N; ++k) {
        std::swap(tmp[i][k], tmp[pivot][k]);
        std::swap(res[i][k], res[pivot][k]);
      }

      T diag = tmp[i][i];
      for (int k = 0; k < N; ++k) {
        tmp[i][k] /= diag;
        res[i][k] /= diag;
      }

      for (int j = 0; j < N; ++j) {
        if (j == i)
          continue;
        T factor = tmp[j][i];
        for (int k = 0; k < N; ++k) {
          tmp[j][k] -= factor * tmp[i][k];
          res[j][k] -= factor * res[i][k];
        }
      }
    }

    return Mat(res);
  }
  T *data() { return vals; }
};

// usage Mat3<double> or Mat3<float>
template <typename T> using Mat3 = Mat<T, 3>;
template <typename T> using Mat4 = Mat<T, 4>;

using Mat3f = Mat<float, 3>;
using Mat4f = Mat<float, 4>;
using Mat3d = Mat<double, 3>;
using Mat4d = Mat<double, 4>;

// agar komutatif
template <typename T, int N>
Mat<T, N> operator*(const T fp, const Mat<T, N> &m) {
  return m * fp;
}

// untuk mengubah matriks 3×3 ke 4×4
template <typename T> Mat<T, 4> mat3_to_mat4(const Mat<T, 3> &m) {
  T res_arr[4 * 4];
  for (int i = 0; i < 16; ++i) {
    if ((i & 3) == 3 || (i >> 2) == 3)
      res_arr[i] = (i == 15) ? 1 : 0;
    else
      res_arr[i] = m[i >> 2][i & 3];
  }
  return Mat<T, 4>(res_arr);
}

/*operasi matriks 4×4 * 3×3
 * rubah dulu yang 3×3 ke 4×4 dengan menambahkan komponen w
 */
template <typename T>
Mat<T, 4> operator*(const Mat<T, 4> &a, const Mat<T, 3> &b) {
  return a * mat3_to_mat4<T>(b);
}

// agar berlaku sebaliknya juga
template <typename T>
Mat<T, 4> operator*(const Mat<T, 3> &a, const Mat<T, 4> &b) {
  return mat3_to_mat4<T>(a) * b;
}

// bakal berguna nanti buat kelas kelas seperti kamera objek atau proyeksi
enum MATRIX_ROTATION_TYPE { EULER, QUATERNION };
enum MATRIX_PROJECTION_TYPE { PERSPECTIVE, ORTHOGRAPHIC, FRUSTUM };

// buat urutan rotasi karena rotasi euler bergantung banget sama urutan
enum EULER_ROTATION_TYPE { ZYX, ZXY, YXZ, YZX, XZY, XYZ };

// View matrix
template <typename T,
          typename = std::enable_if_t<std::is_floating_point_v<T>, float>>
Mat<T, 4> VIEW_MATRIX(const Vec3<T> &eye, const Vec3<T> &center,
                      const Vec3<T> &up = {0, 1, 0},
                      const Vec3<T> &t = {0, 0, 0}) {
  // Forward, Right, dan Up vector
  Vec3<T> f = normalize(center - eye); // forward vector
  Vec3<T> r = normalize(cross(f, up)); // right vector
  Vec3<T> u = cross(r, f);             // real up vector

  return Mat<T, 4>({r.x(), u.x(), -f.x(), 0, r.y(), u.y(), -f.y(), 0, r.z(),
                    u.z(), -f.z(), 0, -(dot(r, eye)) + t.x(),
                    -(dot(u, eye)) + t.y(), dot(f, eye) + t.z(), 1});
}

// Perspective Matrix
template <typename T> Mat<T, 4> PERSPECTIVE_MATRIX(T Fov, T a, T n, T f) {
  T tan_half_fov = std::tan(Fov / 2);
  return Mat<T, 4>({1 / (tan_half_fov * a), 0, 0, 0, 0, 1 / tan_half_fov, 0, 0,
                    0, 0, (f + n) / (f - n), 2 * f * n / (f - n), 0, 0, -1, 0});
}

// Orthographic Matrix
template <typename T>
Mat<T, 4> ORTHOGRAPHIC_MATRIX(T l, T r, T t, T b, T n, T f) {
  return Mat<T, 4>({2 / (r - l), 0, 0, -(r + l) / (r - l), 0, 2 / (t - b), 0,
                    -(t + b) / (t - b), 0, 0, -2 / (f - n), -(f + n) / (f - n),
                    0, 0, 0, 1});
}

// Frustum Matrix
template <typename T> Mat<T, 4> FRUSTUM_MATRIX(T l, T r, T t, T b, T n, T f) {
  return Mat<T, 4>({(2 * n) / (r - l), 0, (r + l) / (r - l), 0, 0,
                    (2 * n) / (t - b), (t + b) / (t - b), 0, 0, 0,
                    (f + n) / (f - n), (2 * f * n) / (f - n), 0, 0, -1, 0});
}

template <typename T>
Mat<T, 3> EULER_ROTATION_MATRIX(const Vec3<T> &rad,
                                const EULER_ROTATION_TYPE &rt) {
  // semua rotasi bergantung pada global axis
  // Urutan terbalik karena Matrix selalu lhs terhadap objek
  // rotasi di sumbu x
  T cos_x = std::cos(rad.x()), sin_x = std::sin(rad.x());
  T cos_y = std::cos(rad.y()), sin_y = std::sin(rad.y());
  T cos_z = std::cos(rad.z()), sin_z = std::sin(rad.z());

  Mat<T, 3> Rx{1, 0, 0, 0, cos_x, -sin_x, 0, sin_x, cos_x};
  // rotasi di sumbu y
  Mat<T, 3> Ry{cos_y, 0, sin_y, 0, 1, 0, -sin_y, 0, cos_y};
  // rotasi di sumbu z
  Mat<T, 3> Rz{cos_z, -sin_z, 0, sin_z, cos_z, 0, 0, 0, 1};
  switch (rt) {
  case ZYX:
    return Rx * Ry * Rz;
  case ZXY:
    return Ry * Rx * Rz;
  case YZX:
    return Rx * Rz * Ry;
  case YXZ:
    return Rz * Rx * Ry;
  case XZY:
    return Rx * Rz * Ry;
  case XYZ:
    return Rz * Ry * Rx;
  }
}

template <typename T> Mat<T, 3> QUATERNION_MATRIX(const Vec3<T> &v, T rad) {
  T s = static_cast<T>(sin(rad / 2)), c = static_cast<T>(cos(rad / 2)),
    x = v.x() * s, y = v.y() * s, z = v.z() * s;

  // Matriks quaternion dihitung
  T mat[9] = {
      1 - 2 * (y * y + z * z), 2 * (x * y - c * z),     2 * (x * z + c * y),
      2 * (x * y + c * z),     1 - 2 * (x * x + z * z), 2 * (y * z - c * x),
      2 * (x * z - c * y),     2 * (y * z + c * x),     1 - 2 * (x * x + y * y),
  };

  return Mat<T, 3>(mat);
}

} // namespace Linear

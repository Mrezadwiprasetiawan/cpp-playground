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
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "vec.hxx"

namespace Linear {

// example usage Mat<double,4> Matrix 4×4 (Cols default-nya sama dengan Rows)
// atau Mat<double,4,3> buat matriks non-persegi 4 baris 3 kolom
template <std::floating_point T, int Rows, int Cols = Rows>
class Mat {
 private:
  T vals[Rows * Cols];

 public:
  explicit Mat() : vals() {}

  // Cramer's Rule, dipanggil dari augmented matrix [A | b] (Rows persamaan,
  // Rows variabel, kolom terakhir hasil). Reuse determinant() yang udah
  // eliminasi gauss + pivoting. Cuma valid kalau Cols == Rows+1 (satu kolom
  // ekstra buat b), makanya static dan gak butuh instance Mat<T,Rows,Rows>.
  static Vec<T, Rows> solve(const Mat &aug) requires(Cols == Rows + 1) {
    Mat<T, Rows, Rows> A;
    Vec<T, Rows>       b;
    for (int i = 0; i < Rows; ++i) {
      for (int j = 0; j < Rows; ++j) A.set_element(i * Rows + j, aug[i][j]);
      b[i] = aug[i][Rows];
    }

    T detA = A.determinant();
    if (detA == 0) throw std::runtime_error("Singular matrix, sistem tidak punya solusi unik");

    Vec<T, Rows> x;
    // loop per kolom -- Acol selalu di-copy fresh dari A asli, bukan dari
    // hasil modifikasi kolom sebelumnya, biar gak rapuh kalau urutannya kebalik
    for (int col = 0; col < Rows; ++col) {
      Mat<T, Rows, Rows> Acol(A);
      for (int row = 0; row < Rows; ++row) Acol.set_element(row * Rows + col, b[row]);
      x[col] = Acol.determinant() / detA;
    }
    return x;
  }

  // copy constructor so that we can copy the matrix dirrectly
  Mat(const Mat &other) = default;
  // copy with assigment operator so that we can copy the matrix without explicitly loopi through the data
  Mat &operator=(const Mat &other) = default;

  // converting copy constructor, agar Mat<T,Rows,Cols> bisa dicopy dari Mat<U,Rows,Cols> dengan tipe elemen berbeda
  template <typename U>
  Mat(const Mat<U, Rows, Cols> &other) {
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j) vals[i * Cols + j] = static_cast<T>(other[i][j]);
  }

  // converting copy assignment, sama seperti di atas tapi untuk operator=
  template <typename U>
  Mat &operator=(const Mat<U, Rows, Cols> &other) {
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j) vals[i * Cols + j] = static_cast<T>(other[i][j]);
    return *this;
  }

  Mat(T (&v)[Rows * Cols]) { set_elements(v); }
  Mat(const std::initializer_list<T> &v) { set_elements(v); }
  Mat(T (&v)[Rows][Cols]) {
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j) vals[Cols * i + j] = v[i][j];
  }

  void to_array(T (&arr)[Rows * Cols]) const {
    for (int i = 0; i < Rows * Cols; ++i) arr[i] = vals[i];
  }

  std::vector<T> to_vector() const { return std::vector<T>(vals, vals + Rows * Cols); }

  void set_elements(T (&v)[Rows * Cols]) {
    for (int i = 0; i < Rows * Cols; ++i) vals[i] = v[i];
  }

  void set_elements(const std::vector<T> &v) {
    assert(v.size() == Rows * Cols);
    for (int i = 0; i < Rows * Cols; ++i) vals[i] = v[i];
  }

  void set_elements(const std::initializer_list<T> &v) {
    assert(v.size() == Rows * Cols);
    auto it = v.begin();
    // tricky menambah i bersamaan dengan menambah iterator it
    for (int i = 0; i < Rows * Cols; ++i, ++it) vals[i] = *it;
  }

  // cuma valid buat matriks persegi
  void set_identity() requires(Rows == Cols) {
    for (int row = 0; row < Rows; ++row)
      for (int col = 0; col < Cols; ++col)
        if (row == col) vals[row * Cols + col] = 1.0;
        else vals[row * Cols + col] = 0;
  }

  void set_element(size_t i, T val) { vals[i] = val; }

  // ==== operasi in-place, method ini langsung memodifikasi object saat ini (void, tanpa return) ====
  // dipakai sebagai basis untuk operator biasa (yang mengembalikan objek baru hasil copy)
  // dan operator penugasan (+=, -=, dst) supaya tidak ada copy yang sia-sia

  template <typename U>
  void add(const Mat<U, Rows, Cols> &m) {
    for (int i = 0; i < Rows * Cols; ++i) vals[i] += static_cast<T>(m[i / Cols][i % Cols]);
  }

  template <typename U>
  void sub(const Mat<U, Rows, Cols> &m) {
    for (int i = 0; i < Rows * Cols; ++i) vals[i] -= static_cast<T>(m[i / Cols][i % Cols]);
  }

  // perkalian matriks in-place cuma valid kalau persegi, karena hasil A(Rows×Cols) * B(Cols×Cols2)
  // bentuknya Rows×Cols2 -- beda dari bentuk asli kecuali Rows==Cols dan Cols2==Cols
  template <typename U>
  requires(Rows == Cols) void mul(const Mat<U, Cols, Cols> &m) {
    T vals_res[Rows * Cols]{};
    // k ini faktor untuk ngurusin perkaliannya
    // sedangkan row dan col untuk indeks hasil akhirnya
    for (int row = 0; row < Rows; ++row)
      for (int col = 0; col < Cols; ++col)
        for (int k = 0; k < Cols; ++k) vals_res[row * Cols + col] += vals[row * Cols + k] * static_cast<T>(m[k][col]);

    for (int i = 0; i < Rows * Cols; ++i) vals[i] = vals_res[i];
  }

  template <typename U>
  requires(std::integral<U> || std::floating_point<U>) void mul(U c) {
    for (int i = 0; i < Rows * Cols; ++i) vals[i] *= c;
  }

  // ==== operator biasa, hasilnya selalu object baru (copy dulu baru dipanggil in-place-nya) ====

  template <typename U>
  requires(std::integral<T> || std::floating_point<T>) Mat operator*(U c) const {
    Mat res(*this);
    res.mul(c);
    return res;
  }

  Vec<T, Cols> operator[](int row) const {
    assert(row >= 0 && row < Rows);
    Vec<T, Cols> res;
    for (int col = 0; col < Cols; ++col) res[col] = vals[row * Cols + col];
    return res;
  }

  template <typename U>  // all of the number type are accept, but doesnt need to check the value is number or not since the vec class already handle it
  Vec<T, Rows> operator*(const Vec<U, Cols> &vn) const {
    Vec<T, Rows> res;
    for (int row = 0; row < Rows; ++row)
      for (int col = 0; col < Cols; ++col) res[row] += vals[row * Cols + col] * static_cast<T>(vn[col]);
    return res;
  }

  // perkalian matriks general, A(Rows×Cols) * B(Cols×Cols2) -> hasilnya Rows×Cols2, bisa beda bentuk dari A
  template <typename U, int Cols2>
  Mat<T, Rows, Cols2> operator*(const Mat<U, Cols, Cols2> &m) const {
    T vals_res[Rows * Cols2]{};
    for (int row = 0; row < Rows; ++row)
      for (int col = 0; col < Cols2; ++col)
        for (int k = 0; k < Cols; ++k) vals_res[row * Cols2 + col] += vals[row * Cols + k] * static_cast<T>(m[k][col]);

    return Mat<T, Rows, Cols2>(vals_res);
  }

  template <typename U>
  Mat operator+(const Mat<U, Rows, Cols> &m) const {
    Mat res(*this);
    res.add(m);
    return res;
  }

  template <typename U>
  Mat operator-(const Mat<U, Rows, Cols> &m) const {
    Mat res(*this);
    res.sub(m);
    return res;
  }

  // ==== operator penugasan, langsung pakai method in-place di atas, tanpa copy dulu ====

  template <typename U>
  Mat &operator+=(const Mat<U, Rows, Cols> &m) {
    add(m);
    return *this;
  }

  template <typename U>
  Mat &operator-=(const Mat<U, Rows, Cols> &m) {
    sub(m);
    return *this;
  }

  template <typename U>
  requires(Rows == Cols) Mat &operator*=(const Mat<U, Cols, Cols> &m) {
    mul(m);
    return *this;
  }

  template <typename U>
  requires(std::integral<U> || std::floating_point<U>) Mat &operator*=(U c) {
    mul(c);
    return *this;
  }

  // submatrix (Rows-1)×(Cols-1) hasil menghapus baris `row` dan kolom `col`, cuma valid buat matriks persegi
  Mat<T, Rows - 1, Cols - 1> minor(int row, int col) const requires(Rows == Cols) {
    Mat<T, Rows - 1, Cols - 1> res;
    int                        ri = 0;
    for (int i = 0; i < Rows; ++i) {
      if (i == row) continue;
      int ci = 0;
      for (int j = 0; j < Cols; ++j) {
        if (j == col) continue;
        res.set_element(ri * (Cols - 1) + ci, vals[i * Cols + j]);
        ++ci;
      }
      ++ri;
    }
    return res;
  }

  // cofactor elemen di baris `row` kolom `col`, cuma valid buat matriks persegi
  T cofactor(int row, int col) const requires(Rows == Cols) {
    T sign = ((row + col) % 2 == 0) ? T(1) : T(-1);
    return sign * minor(row, col).determinant();
  }

  // in-place, langsung ubah baris jadi kolom dan kolom jadi baris pada object ini, cuma valid buat matriks persegi
  void transpose() requires(Rows == Cols) {
    T tmpvals[Rows * Cols];
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j) tmpvals[i * Cols + j] = vals[j * Cols + i];
    for (int i = 0; i < Rows * Cols; ++i) vals[i] = tmpvals[i];
  }

  // general, valid buat ukuran apapun (termasuk non-persegi), makanya bentuknya berubah jadi Cols×Rows
  Mat<T, Cols, Rows> transposed() const {
    T tmpvals[Rows * Cols];
    for (int i = 0; i < Cols; ++i)
      for (int j = 0; j < Rows; ++j) tmpvals[i * Rows + j] = vals[j * Cols + i];
    return Mat<T, Cols, Rows>(tmpvals);
  }

  // eliminasi gauss, cuma valid buat matriks persegi
  T determinant() const requires(Rows == Cols) {
    T det = 1;
    T tmpvals[Rows * Cols];
    to_array(tmpvals);
    for (int i = 0; i < Rows; ++i) {
      // cari baris dengan elemen terbesar di kolom i (pivoting)
      int maxRow = i;
      for (int j = i + 1; j < Rows; ++j)
        if (std::abs(tmpvals[j * Cols + i]) > std::abs(tmpvals[maxRow * Cols + i])) maxRow = j;
      // tukar baris i dengan baris maxRow jika perlu
      if (i != maxRow) {
        for (int k = 0; k < Cols; ++k) std::swap(tmpvals[i * Cols + k], tmpvals[maxRow * Cols + k]);
        det = -det;  // Perubahan tanda jika ada pertukaran baris
      }
      // jika elemen diagonalnya 0, matriks tidak invertible, det = 0
      if (tmpvals[i * Cols + i] == 0) return 0;
      // eliminasi Gauss untuk baris di bawahnya
      for (int j = i + 1; j < Rows; ++j) {
        T factor = tmpvals[j * Cols + i] / tmpvals[i * Cols + i];
        for (int k = i; k < Cols; ++k) tmpvals[j * Cols + k] -= factor * tmpvals[i * Cols + k];
      }
      // Kalikan elemen diagonal untuk determinan
      det *= tmpvals[i * Cols + i];
    }
    return det;
  }

  // in-place, ubah object ini jadi inverse-nya. N==2/3 pakai adjugate (lebih cepat buat ukuran segini),
  // selain itu pakai eliminasi gauss-jordan yang lebih scalable buat N lebih besar. Cuma valid buat matriks persegi.
  void inverse() requires(Rows == Cols) {
    if constexpr (Rows == 2 || Rows == 3) {
      T det = determinant();
      if (det == 0) throw std::runtime_error("Singular matrix");
      adjugate();
      for (int i = 0; i < Rows * Cols; ++i) vals[i] /= det;
    } else {
      T res[Rows][Cols], tmp[Rows][Cols];
      for (int i = 0; i < Rows; ++i)
        for (int j = 0; j < Cols; ++j) {
          tmp[i][j] = vals[i * Cols + j];
          res[i][j] = (i == j ? 1 : 0);
        }

      for (int i = 0; i < Rows; ++i) {
        int pivot = i;
        for (int j = i + 1; j < Rows; ++j)
          if (std::abs(tmp[j][i]) > std::abs(tmp[pivot][i])) pivot = j;
        if (tmp[pivot][i] == 0) throw std::runtime_error("Singular matrix");

        for (int k = 0; k < Cols; ++k) {
          std::swap(tmp[i][k], tmp[pivot][k]);
          std::swap(res[i][k], res[pivot][k]);
        }

        T diag = tmp[i][i];
        for (int k = 0; k < Cols; ++k) {
          tmp[i][k] /= diag;
          res[i][k] /= diag;
        }

        for (int j = 0; j < Rows; ++j) {
          if (j == i) continue;
          T factor = tmp[j][i];
          for (int k = 0; k < Cols; ++k) {
            tmp[j][k] -= factor * tmp[i][k];
            res[j][k] -= factor * res[i][k];
          }
        }
      }

      for (int i = 0; i < Rows; ++i)
        for (int j = 0; j < Cols; ++j) vals[i * Cols + j] = res[i][j];
    }
  }

  // copy dulu baru inverse, dipakai kalau object aslinya ga boleh ikut berubah
  Mat inverted() const requires(Rows == Cols) {
    Mat res(*this);
    res.inverse();
    return res;
  }

  // in-place, ubah object ini jadi adjugate-nya (transpose dari matriks cofactor), cuma valid buat matriks persegi
  void adjugate() requires(Rows == Cols) {
    T res[Rows * Cols];
    for (int i = 0; i < Rows; ++i)
      for (int j = 0; j < Cols; ++j)
        // ditranspose langsung disini, jadi ambil cofactor(j, i) bukan cofactor(i, j)
        res[i * Cols + j] = cofactor(j, i);
    for (int i = 0; i < Rows * Cols; ++i) vals[i] = res[i];
  }

  // copy dulu baru adjugate, dipakai kalau object aslinya ga boleh ikut berubah
  Mat adjugated() const requires(Rows == Cols) {
    Mat res(*this);
    res.adjugate();
    return res;
  }

  T *data() { return vals; }

  void debug(const std::string &name) {
    std::cout << name << " = \n";
    int index = 0;
    for (int y = 0; y < Rows; ++y) {
      std::cout << "[\t";
      for (int x = 0; x < Cols; ++x, ++index) std::cout << vals[index] << "\t";
      std::cout << "]";
      std::cout << "\n";
    }
  }
};

// usage Mat3<double> or Mat3<float>
template <typename T>
using Mat3 = Mat<T, 3>;
template <typename T>
using Mat4 = Mat<T, 4>;

using Mat3f = Mat<float, 3>;
using Mat4f = Mat<float, 4>;
using Mat3d = Mat<double, 3>;
using Mat4d = Mat<double, 4>;

// agar komutatif
template <typename T, int Rows, int Cols>
Mat<T, Rows, Cols> operator*(const T fp, const Mat<T, Rows, Cols> &m) {
  return m * fp;
}

// untuk mengubah matriks 3×3 ke 4×4
template <typename T>
Mat<T, 4> mat3_to_mat4(const Mat<T, 3> &m) {
  T res_arr[4 * 4];
  for (int i = 0; i < 16; ++i) {
    if ((i & 3) == 3 || (i >> 2) == 3) res_arr[i] = (i == 15) ? 1 : 0;
    else res_arr[i] = m[i >> 2][i & 3];
  }
  return Mat<T, 4>(res_arr);
}

// untuk trim matrix 4×4 ke 3×3
template <typename T>
Mat<T, 3> mat4_to_mat3(const Mat<T, 4> &m) {
  T res_arr[3 * 3];
  for (int i = 0; i < 9; ++i) {
    if ((i & 3) == 2 || (i >> 2) == 2) res_arr[i] = 0;  // menghilangkan kolom dan baris ke-3
    else res_arr[i] = m[i >> 2][i & 3];
  }
  return Mat<T, 3>(res_arr);
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
template <typename T>
Mat<T, 4> VIEW_MATRIX(const Vec3<T> &eye, const Vec3<T> &center, const Vec3<T> &up = {0, 1, 0}, const Vec3<T> &t = {0, 0, 0}) {
  // Forward, Right, dan Up vector
  Vec3<T> f = normalize(center - eye);  // forward vector
  Vec3<T> r = normalize(cross(f, up));  // right vector
  Vec3<T> u = cross(r, f);              // real up vector

  return Mat<T, 4>(
      {r.x(), u.x(), -f.x(), 0, r.y(), u.y(), -f.y(), 0, r.z(), u.z(), -f.z(), 0, -(dot(r, eye)) + t.x(), -(dot(u, eye)) + t.y(), dot(f, eye) + t.z(), 1});
}

// Perspective Matrix
template <typename T>
Mat<T, 4> PERSPECTIVE_MATRIX(T Fov, T a, T n, T f) {
  T tan_half_fov = std::tan(Fov / 2);
  return Mat<T, 4>({1 / (tan_half_fov * a), 0, 0, 0, 0, 1 / tan_half_fov, 0, 0, 0, 0, (f + n) / (f - n), 2 * f * n / (f - n), 0, 0, -1, 0});
}

// Orthographic Matrix
template <typename T>
Mat<T, 4> ORTHOGRAPHIC_MATRIX(T l, T r, T t, T b, T n, T f) {
  return Mat<T, 4>({2 / (r - l), 0, 0, -(r + l) / (r - l), 0, 2 / (t - b), 0, -(t + b) / (t - b), 0, 0, -2 / (f - n), -(f + n) / (f - n), 0, 0, 0, 1});
}

// Frustum Matrix
template <typename T>
Mat<T, 4> FRUSTUM_MATRIX(T l, T r, T t, T b, T n, T f) {
  return Mat<T, 4>(
      {(2 * n) / (r - l), 0, (r + l) / (r - l), 0, 0, (2 * n) / (t - b), (t + b) / (t - b), 0, 0, 0, (f + n) / (f - n), (2 * f * n) / (f - n), 0, 0, -1, 0});
}

template <typename T>
Mat<T, 3> EULER_ROTATION_MATRIX(const Vec3<T> &rad, const EULER_ROTATION_TYPE &rt) {
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
    default: throw std::invalid_argument("UNKNOWN EULER ROTATION MATRIX TYPE");
    case ZYX: return Rx * Ry * Rz;
    case ZXY: return Ry * Rx * Rz;
    case YZX: return Rx * Rz * Ry;
    case YXZ: return Rz * Rx * Ry;
    case XZY: return Rx * Rz * Ry;
    case XYZ: return Rz * Ry * Rx;
  }
}

template <typename T>
Mat<T, 3> QUATERNION_MATRIX(const Vec3<T> &v, T rad) {
  T s = static_cast<T>(sin(rad / 2)), c = static_cast<T>(cos(rad / 2)), x = v.x() * s, y = v.y() * s, z = v.z() * s;

  // Matriks quaternion dihitung
  T mat[9] = {
      1 - 2 * (y * y + z * z), 2 * (x * y - c * z), 2 * (x * z + c * y), 2 * (x * y + c * z),     1 - 2 * (x * x + z * z),
      2 * (y * z - c * x),     2 * (x * z - c * y), 2 * (y * z + c * x), 1 - 2 * (x * x + y * y),
  };

  return Mat<T, 3>(mat);
}

}  // namespace Linear

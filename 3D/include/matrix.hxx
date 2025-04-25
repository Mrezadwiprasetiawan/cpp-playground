#pragma once

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <custom_trait.hxx>
#include <initializer_list>
#include <stdexcept>
#include <vec.hxx>
#include <vector>

namespace l3d {
template <typename T, int N, typename = ifel_trait_t<is_fp<T>, float>>
class Mat {
  T val[N * N];

 public:
  Mat() : val() {}

  Mat(T (&v)[N * N]) { set_element(v); }

  Mat(const std::initializer_list<T> &v) { set_element(v); }

  Mat(T (&v)[N][N]) {
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j) val[N * i + j] = v[i][j];
  }

  void to_array(T (&arr)[N * N]) const {
    for (int i = 0; i < N * N; ++i) arr[i] = val[i];
  }

  std::vector<T> to_vector() const { return std::vector<T>(val, val + N * N); }

  void set_element(T (&v)[N * N]) {
    for (int i = 0; i < N * N; ++i) val[i] = v[i];
  }

  void set_element(const std::vector<T> &v) {
    assert(v.size() == N * N);
    for (int i = 0; i < N * N; ++i) val[i] = v[i];
  }

  void set_element(const std::initializer_list<T> &v) {
    assert(v.size() == N * N);
    auto it = v.begin();
    // tricky menambah i bersamaan dengan menambah iterator it
    for (int i = 0; i < N * N; ++i, ++it) val[i] = *it;
  }

  void set_identity() {
    for (int row = 0; row < N; ++row) for (int col = 0; col < N; ++col)
      if (row == col) val[row * N + col] = 1.0;
      else val[row * N + col] = 0;
  }
  template <typename U>
  T dot_product(const Mat<U, N> &m) {
    U val_m[N * N];
    m.to_array(val_m);
    T result = 0;
    for (int i = 0; i < N * N; ++i) result += val[i] * val_m[i];
    return result;
  }

  template <typename U>
  Mat operator*(U fp) const {
    T val_cp[N * N];
    for (int i = 0; i < N * N; ++i) val_cp[i] = val[i] * fp;
    return Mat(val_cp);
  }

  template <typename U>
  Mat operator*(const Mat<U, N> &m) const {
    T val_res[N * N]{};
    U val_m[N * N];
    m.to_array(val_m);
    // k ini faktor untuk ngurusin perkaliannya
    // sedangkan row dan col untuk indeks hasil akhirnya
    for (int row = 0; row < N; ++row)
      for (int col = 0; col < N; ++col)
        for (int k = 0; k < N; ++k)
          val_res[row * N + col] += val[row * N + k] * val_m[k * N + col];

    return Mat(val_res);
  }

  template <typename U>
  Mat operator+(const Mat<U, N> &m) const {
    T val_res[N * N]{};
    T val_m[N * N];
    m.to_array(val_m);
    for (int i = 0; i < N * N; ++i) val_res[i] = val[i] + val_m[i];
    return Mat(val_res);
  }

  template <typename U>
  Mat operator-(const Mat<U, N> &m) const {
    T val_res[N * N]{};
    T val_m[N * N];
    m.to_array(val_m);
    for (int i = 0; i < N * N; ++i) val_res[i] = val[i] - val_m[i];
    return Mat(val_res);
  }

  // row
  Vec<T, N> operator[](size_t index) {
    T arr[N];
    for (int i = 0; i < N; ++i) arr[i] = val[index * N + i];
    return Vec<T, N>(arr);
  }

  // eliminasi gauss
  T determinant() const {
    T det = 1;
    T tmpval[N * N];
    to_array(tmpval);
    for (int i = 0; i < N; ++i) {
      // cari baris dengan elemen terbesar di kolom i (pivoting)
      int maxRow = i;
      for (int j = i + 1; j < N; ++j)
        if (std::abs(tmpval[j * N + i]) > std::abs(tmpval[maxRow * N + i]))
          maxRow = j;
      // tukar baris i dengan baris maxRow jika perlu
      if (i != maxRow) {
        for (int k = 0; k < N; ++k)
          std::swap(tmpval[i * N + k], tmpval[maxRow * N + k]);
        det = -det;  // Perubahan tanda jika ada pertukaran baris
      }
      // jika elemen diagonalnya 0, matriks tidak invertible, det = 0
      if (tmpval[i * N + i] == 0) return 0;
      // eliminasi Gauss untuk baris di bawahnya
      for (int j = i + 1; j < N; ++j) {
        T factor = tmpval[j * N + i] / tmpval[i * N + i];
        for (int k = i; k < N; ++k)
          tmpval[j * N + k] -= factor * tmpval[i * N + k];
      }
      // Kalikan elemen diagonal untuk determinan
      det *= tmpval[i * N + i];
    }
    return det;
  }

  Mat transpose() const {
    // tukar baris menjadi kolom dan kolom menjadi baris
    T tmpval[N * N];
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j) tmpval[i * N + j] = val[j * N + i];
    return Mat(tmpval);
  }

  Mat inverse() const {
    T res[N][N], tmp[N][N];
    for (int i = 0; i < N; ++i)
      for (int j = 0; j < N; ++j) {
        tmp[i][j] = val[i * N + j];
        res[i][j] = (i == j ? 1 : 0);
      }

    for (int i = 0; i < N; ++i) {
      int pivot = i;
      for (int j = i + 1; j < N; ++j)
        if (std::abs(tmp[j][i]) > std::abs(tmp[pivot][i])) pivot = j;
      if (tmp[pivot][i] == 0) throw std::runtime_error("Singular matrix");

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
        if (j == i) continue;
        T factor = tmp[j][i];
        for (int k = 0; k < N; ++k) {
          tmp[j][k] -= factor * tmp[i][k];
          res[j][k] -= factor * res[i][k];
        }
      }
    }

    return Mat(res);
  }
};

using Mat3f = Mat<float, 3>;
using Mat4f = Mat<float, 4>;
using Mat3d = Mat<double, 3>;
using Mat4d = Mat<double, 4>;

// agar komutatif
template <typename T, int N>
Mat<T, N> operator*(const T fp, const Mat<T, N> &m) {
  return m * fp;
}

enum MATRIX_ROTATION_TYPE { EULER, EULER2, QUATERNION };
enum MATRIX_PROJECTION_TYPE { PERSPECTIVE, ORTHOGRAPHIC };

template <typename T, typename = ifel_trait_t<is_fp<T>, float>>
class Matrix {
  Matrix() = delete;

 public:
  static void set_as_model_matrix(Mat<T, 4> &m, T tx = 0, T ty = 0, T tz = 0,
                                  T rx = 0, T ry = 0, T rz = 0, T sx = 1,
                                  T sy = 1, T sz = 1);
  static void set_as_view_matrix(Mat<T, 4> &m, T tx, T ty, T tz, T cx, T cy,
                                 T cz, T ux, T uy, T uz) {}
  static void set_as_perspective_matrix(Mat<T, 4> &m, T Fov, T a, T n, T f);
  static void set_as_orthographic_matrix(Mat<T, 4> &m, T l, T r, T t, T b, T n,
                                         T f);
  static void set_as_matrix_rotation(Mat<T, 4> &m, MATRIX_ROTATION_TYPE mrt,
                                     T degree, T px, T py, T pz);
  static void set_as_matrix_translation(Mat<T, 4> &m, T tx, T ty, T tz);
};

}  // namespace l3d

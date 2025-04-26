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

// example usage Mat<double,4> Matrix 4 * 4 with double element type
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
    for (int row = 0; row < N; ++row)
      for (int col = 0; col < N; ++col)
        if (row == col) val[row * N + col] = 1.0;
        else
          val[row * N + col] = 0;
  }

  template <typename U>
  Mat operator*(U fp) const {
    T val_cp[N * N];
    for (int i = 0; i < N * N; ++i) val_cp[i] = val[i] * fp;
    return Mat(val_cp);
  }

  Vec<T, N> operator*(const Vec<T, N> &vn) const {
    Vec<T, N> res;
    for (int row = 0; row < N; ++row)
      for (int col = 0; col < N; ++col)
        res[row] += val[row * N + col] * vn[col];
    return res;
  }

  template <typename U>
  Mat operator*(const Mat<U, N> &m) const {
    T val_res[N * N]{};
    // k ini faktor untuk ngurusin perkaliannya
    // sedangkan row dan col untuk indeks hasil akhirnya
    for (int row = 0; row < N; ++row)
      for (int col = 0; col < N; ++col)
        for (int k = 0; k < N; ++k)
          val_res[row * N + col] += val[row * N + k] * m.data()[k * N + col];

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
    for (int i = 0; i < N * N; ++i) val_res[i] = val[i] - m.data()[i];
    return Mat(val_res);
  }

  // row
  Vec<T, N> operator[](size_t index) const {
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
  const T *data() const { return val; }
};

// usage Mat3<double> or Mat3<float>
template <typename T>
using Mat3 = Mat<T, 3>;
template <typename T>
using Mat4 = Mat<T, 3>;

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
template <typename T>
Mat<T, 4> Mat3_to_Mat4(const Mat<T, 4> &m) {
  T res_arr[4 * 4];
  for (int i = 0; i < 16; ++i) {
    if ((i & 3) == 3 || (i >> 2) == 3) res_arr[i] = (i == 15) ? 1 : 0;
    else
      res_arr[i] = m.data()[(i & 3) * 4 + (i >> 2)];
  }
  return Mat<T, 4>(res_arr);
}

// operasi matriks 4×4 * 3×3
// rubah dulu yang 3×3 ke 4×4 dengan menambahkan komponen w
template <typename T>
Mat<T, 4> operator*(const Mat<T, 4> &a, const Mat<T, 3> &b) {
  return a * Mat3_to_Mat4<T>(b);
}

// agar berlaku sebaliknya juga
template <typename T>
Mat<T, 4> operator*(const Mat<T, 3> &a, const Mat<T, 4> &b) {
  return Mat3_to_Mat4<T>(a) * b;
}

// bakal berguna nanti buat kelas kelas seperti kamera objek atau proyeksi
enum MATRIX_ROTATION_TYPE { EULER, QUATERNION };
enum MATRIX_PROJECTION_TYPE { PERSPECTIVE, ORTHOGRAPHIC, FRUSTUM };

// buat urutan rotasi karena rotasi euler bergantung banget sama urutan
enum EULER_ROTATION_TYPE { ZYX, ZXY, YXZ, YZX, XZY, XYZ };

// View matrix
template <typename T, typename = ifel_trait_t<is_fp<T>, float>>
Mat<T, 4> VIEW_MATRIX(Vec3<T> eye, Vec3<T> center, Vec3<T> up, Vec3<T> t) {
  // Forward, Right, dan Up vector
  Vec3<T> f = normalize(center - eye);  // forward vector
  Vec3<T> r = normalize(cross(f, up));  // right vector
  Vec3<T> u = cross(r, f);              // real up vector

  return Mat<T, 4>({r.x(), u.x(), -f.x(), 0, r.y(), u.y(), -f.y(), 0, r.z(),
                    u.z(), -f.z(), 0, -(dot(r, eye)) + t.x(),
                    -(dot(u, eye)) + t.y(), dot(f, eye) + t.z(), 1});
}

// Perspective Matrix
template <typename T>
Mat<T, 4> PERSPECTIVE_MATRIX(T Fov, T a, T n, T f) {
  T tan_half_fov = tan(Fov / 2);
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
template <typename T>
Mat<T, 4> FRUSTUM_MATRIX(T l, T r, T t, T b, T n, T f) {
  return Mat<T, 4>({(2 * n) / (r - l), 0, (r + l) / (r - l), 0, 0,
                    (2 * n) / (t - b), (t + b) / (t - b), 0, 0, 0,
                    (f + n) / (f - n), (2 * f * n) / (f - n), 0, 0, -1, 0});
}

/* untuk rotasi hanya kurang dari 2 sumbu, tinggal dipass parametee d pada
 * sumbu yang bukan ke 0 enumnya jadi bebas untuk sumbu hanya 1 dan untuk sumbu
 * 2 enumnya jadi punya tiga pilihan nee ene atau een dengan e adalah sumbu yang
 * aktif
 *
 * kok bisa? karena kalo nilai degreenya 0 sinnya juga 0 cosnya jadi 1, jadi
 * matriks identitas deh
 */
template <typename T>
Mat<T, 4> EULER_ROTATION_MATRIX(const Vec3<T> &d,
                                const EULER_ROTATION_TYPE &rt) {
  // semua rotasi bergantung pada global axis
  // Urutan terbalik karena Matrix selalu lhs terhadap objek
  // rotasi di sumbu x
  Mat<T, 3> Rx{1, 0, 0, 0, cos(d.x()), -sin(d.x()), 0, sin(d.x()), cos(d.x())};
  // rotasi di sumbu y
  Mat<T, 3> Ry{cos(d.y()), 0, -sin(d.y()), 0, 1, 0, sin(d.y()), 0, cos(d.y())};
  // rotasi di sumbu z
  Mat<T, 3> Rz{cos(d.z()), -sin(d.z()), 0, sin(d.z()), sin(d.z()), 0, 0, 0, 1};
  switch (rt) {
    case ZYX: return Mat3_to_Mat4(Rx * Ry * Rz);
    case ZXY: return Mat3_to_Mat4(Ry * Rx * Rz);
    case YZX: return Mat3_to_Mat4(Rx * Rz * Ry);
    case YXZ: return Mat3_to_Mat4(Rz * Rx * Ry);
    case XZY: return Mat3_to_Mat4(Rx * Rz * Ry);
    case XYZ: return Mat3_to_Mat4(Rz * Ry * Rx);
  }
}

// TODO : implement quaternion Matrix

}  // namespace l3d

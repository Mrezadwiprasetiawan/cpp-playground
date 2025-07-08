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

#include <debugger.hxx>
#include <iomanip>
#include <ios>
#include <iostream>
#include <matrix.hxx>
#include <object.hxx>
#include <type_traits>
#include <vec.hxx>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

template <typename T, int N>
void print(const Linear::Vec<T, N> &vn, int n_indent) {
  using namespace std;
  std::string indent = "";
  for (int i = 0; i < n_indent; ++i) indent += "\t";
  cout << indent;
  for (int i = 0; i < N; ++i) cout << vn[i] << " ";
  cout << endl;
}
template <typename T, int N>
void print(const Linear::Mat<T, N> &m, int n_indent) {
  using namespace std;
  bool is_float = std::is_same_v<T, float>;
  ios  oldState(nullptr);
  oldState.copyfmt(cout);
  cout << fixed << setprecision(is_float ? 6 : 15) << endl;
  std::string indent = "";
  for (int i = 0; i < n_indent; ++i) indent += "\t";
  for (int i = 0; i < N; ++i) {
    cout << indent;
    for (int j = 0; j < N; ++j) cout << m[j][i] << "\t";
    cout << endl;
  }
  cout.copyfmt(oldState);
  cout << endl;
}

int main() {
  using namespace std;
  // test Vec with Vec3
  Linear::Vec3f v3({1.1f, 2.5f, 3.0f});
  cout << "tes Vec pake Vec3 v3" << endl;
  cout << "elemen v3\t:" << endl;
  print(v3, 1);

  Linear::Mat3f m3({1.1f, 1.3f, 5.7f, 9.1f, 3.2f, 8.1f, 0.1f, 5.1f, 7.7f});

  // Matriks kedua (m3_2) untuk operasi + dan *
  Linear::Mat3f m3_2({2.0f, 3.0f, 1.0f, 4.0f, 1.0f, 6.0f, 0.5f, 2.5f, 1.5f});

  cout << "elemen m3:" << endl;
  print(m3, 1);

  cout << "hasil m3 * v3\t:" << endl;
  Linear::Vec3f mul_m3_v3 = m3 * v3;
  print(mul_m3_v3, 1);

  cout << "elemen m3_2:" << endl;
  print(m3_2, 1);

  // Operasi m3 + m3_2
  Linear::Mat3f sum = m3 + m3_2;
  cout << "elemen setelah operasi m3 + m3_2:" << endl;
  print(sum, 1);

  // Operasi m3 * m3_2
  cout << "elemen setelah operasi m3 * m3_2" << endl;
  Linear::Mat3f product = m3 * m3_2;
  print(product, 1);

  cout << "determinant m3 = " << m3.determinant() << endl;

  // testing view matrix
  Linear::Vec3f eye{0, 0, 5}, lookAt{0, 0, 0};
  Linear::Mat4f vm = Linear::VIEW_MATRIX<float>(eye, lookAt);
  cout << "elemen view matrix dengan\t:" << endl;
  cout << "\teye\t:" << endl;
  print(eye, 2);
  cout << "\tlookAt\t:" << endl;
  print(lookAt, 2);
  cout << "\telemennya\t:" << endl;
  print(vm, 2);
  cout << "test mat3 ke mat 4 dari m3\t:" << endl;
  print(mat3_to_mat4(m3), 1);
  cout << "Matrix euler dengan 0,1,0\t:" << endl;
  print(Linear::EULER_ROTATION_MATRIX<float>({0, 1, 0}, l3d::EULER_ROTATION_TYPE::XYZ), 1);
  cout << "Matrix quaternion dengan 0,1,0 dan degree = pi \t:" << endl;
  print(Linear::QUATERNION_MATRIX<float>({0, 1, 0}, M_PI), 1);

  cout << "Test obj3d\t:" << endl;
  vector<Linear::Vec3f>       pos{{-1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
  vector<Linear::Vec3<short>> face{{0, 1, 2}};
  l3d::Object3D<float, short> obj(pos, face);
  // rotasi pertama sebenernya ga ada bedanya global dan lokal
  obj.rotate_global({0, 1, 0}, M_PI / 2);
  obj.translate_local({0, 0, 1});
  l3d::Debugger<float, short> debugger;
  debugger.add_object("obj1", obj);
  debugger.run();

  return 0;
}

#include <cmath>
#include <custom_trait.hxx>
#include <iomanip>
#include <ios>
#include <iostream>
#include <matrix.hxx>
#include <vec.hxx>

template <typename T, int N>
void print(const l3d::Vec<T, N> &vn, int n_indent) {
  using namespace std;
  std::string indent = "";
  for (int i = 0; i < n_indent; ++i) indent += "\t";
  cout << indent;
  for (int i = 0; i < N; ++i) cout << vn[i] << " ";
  cout << endl;
}
template <typename T, int N>
void print(const l3d::Mat<T, N> &m, int n_indent) {
  using namespace std;
  bool is_float = ::is_same<T, float>;
  ios oldState(nullptr);
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
  l3d::Vec3f v3({1.1f, 2.5f, 3.0f});
  cout << "tes Vec pake Vec3 v3" << endl;
  cout << "elemen v3\t:" << endl;
  print(v3, 1);

  l3d::Mat3f m3({1.1f, 1.3f, 5.7f, 9.1f, 3.2f, 8.1f, 0.1f, 5.1f, 7.7f});

  // Matriks kedua (m3_2) untuk operasi + dan *
  l3d::Mat3f m3_2({2.0f, 3.0f, 1.0f, 4.0f, 1.0f, 6.0f, 0.5f, 2.5f, 1.5f});

  cout << "elemen m3:" << endl;
  print(m3, 1);

  cout << "hasil m3 * v3\t:" << endl;
  l3d::Vec3f mul_m3_v3 = m3 * v3;

  cout << "elemen m3_2:" << endl;
  print(m3_2, 1);

  // Operasi m3 + m3_2
  l3d::Mat3f sum = m3 + m3_2;
  cout << "elemen setelah operasi m3 + m3_2:" << endl;
  print(sum, 1);

  // Operasi m3 * m3_2
  cout << "elemen setelah operasi m3 * m3_2" << endl;
  l3d::Mat3f product = m3 * m3_2;
  print(product, 1);

  cout << "determinant m3 = " << m3.determinant() << endl;

  // testing view matrix
  l3d::Vec3f eye{0, 0, 5}, lookAt{0, 0, 0};
  l3d::Mat4f vm = l3d::VIEW_MATRIX<float>(eye, lookAt);
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
  print(l3d::EULER_ROTATION_MATRIX<float>({0, 1, 0},
                                          l3d::EULER_ROTATION_TYPE::XYZ),
        1);
  cout << "Matrix quaternion dengan 0,1,0 dan degree = pi \t:" << endl;
  print(l3d::QUATERNION_MATRIX<float>({0, 1, 0}, M_PI), 1);

  return 0;
}

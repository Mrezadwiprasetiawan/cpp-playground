#include <iostream>
#include <matrix.hxx>
#include <vec.hxx>

template <typename T, int N>
void print(const l3d::Vec<T, N> &vn, int n_indent) {
  using namespace std;
  std::string indent = "";
  for (int i = 0; i < n_indent; ++i) indent += "\t";
  cout << indent;
  for (int i = 0; i < N; ++i) cout << vn[i] << "\t";
  cout << endl;
}
template <typename T, int N>
void print(const l3d::Mat<T, N> &m, int n_indent) {
  using namespace std;
  std::string indent = "";
  for (int i = 0; i < n_indent; ++i) indent += "\t";
  for (int i = 0; i < N; ++i) {
    cout << indent;
    for (int j = 0; j < N; ++j) cout << m[j][i] << "\t";
    cout << endl;
  }
  cout << endl;
}

int main() {
  using namespace std;
  // test Vec with Vec3
  l3d::Vec3d v3({1.1f, 2.5f, 3.0f});
  cout << "tes Vec pake Vec3 v3" << endl;
  cout << "elemen v3\t:" << endl;
  cout << "\t";
  for (int i = 0; i < 3; ++i) cout << v3[i] << " ";
  cout << endl;

  l3d::Mat3d m3({1.1f, 1.3f, 5.7f, 9.1f, 3.2f, 8.1f, 0.1f, 5.1f, 7.7f});

  // Matriks kedua (m3_2) untuk operasi + dan *
  l3d::Mat3d m3_2({2.0f, 3.0f, 1.0f, 4.0f, 1.0f, 6.0f, 0.5f, 2.5f, 1.5f});

  cout << "elemen m3:" << endl;
  print(m3, 1);

  cout << "hasil m3 * v3\t:" << endl;
  l3d::Vec3d mul_m3_v3 = m3 * v3;

  cout << "elemen m3_2:" << endl;
  print(m3_2, 1);

  // Operasi m3 + m3_2
  l3d::Mat3d sum = m3 + m3_2;
  cout << "elemen setelah operasi m3 + m3_2:" << endl;
  print(sum, 1);

  // Operasi m3 * m3_2
  cout << "elemen setelah operasi m3 * m3_2" << endl;
  l3d::Mat3d product = m3 * m3_2;
  print(product, 1);

  cout << "determinant m3 = " << m3.determinant() << endl;

  // testing view matrix
  l3d::Vec3d eye{0, 0, 5}, lookAt{0, 0, 0};
  l3d::Mat4f vm = l3d::VIEW_MATRIX<float>(eye[0], eye[1], eye[2], lookAt[0],
                                          lookAt[1], lookAt[2]);
  cout << "elemen view matrix dengan\t:" << endl;
  cout << "\teye\t:" << endl;
  print(eye, 2);
  cout << "\tlookAt\t:" << endl;
  print(lookAt, 2);
  cout << "\telemennya\t:" << endl;
  print(vm, 2);

  return 0;
}

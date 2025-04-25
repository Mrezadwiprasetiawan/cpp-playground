#include <iostream>
#include <matrix.hxx>
#include <vec.hxx>

int main() {
  using namespace std;
  //test Vec with Vec3
  l3d::Vec3f v3({1.1f, 2.5f, 3.0f});
  cout << "tes Vec pake Vec3 v3" << endl;
  cout << "elemen v3\t:" << endl;
  cout << "\t";
  for(int i = 0; i < 3; ++i) cout << v3[i] << " ";
  cout << endl;

  l3d::Mat3f m3({
      1.1f, 1.3f, 5.7f,
      9.1f, 3.2f, 8.1f,
      0.1f, 5.1f, 7.7f
  });

  // Matriks kedua (m3_2) untuk operasi + dan *
  l3d::Mat3f m3_2({
      2.0f, 3.0f, 1.0f,
      4.0f, 1.0f, 6.0f,
      0.5f, 2.5f, 1.5f
  });

  cout << "elemen m3:" << endl;
  for (int i = 0; i < 3; ++i) {
    cout << "\t";
    for (int j = 0; j < 3; ++j) cout << m3[i][j] << " ";
    cout << endl;
  }

  cout << "hasil m3 * v3\t:" << endl;
  cout << "\t";
  l3d::Vec3f mul_m3_v3 = m3 * v3;
  for(int i = 0; i < 3; ++i) cout << mul_m3_v3[i] << " ";
  cout << endl;

  cout << "elemen m3_2:" << endl;
  for (int i = 0; i < 3; ++i) {
    cout << "\t";
    for (int j = 0; j < 3; ++j) cout << m3_2[i][j] << " ";
    cout << endl;
  }

  // Operasi m3 + m3_2
  l3d::Mat3f sum = m3 + m3_2;
  cout << "elemen setelah operasi m3 + m3_2:" << endl;
  for (int i = 0; i < 3; ++i) {
    cout << "\t";
    for (int j = 0; j < 3; ++j) cout << sum[i][j] << " ";
    cout << endl;
  }

  // Operasi m3 * m3_2
  l3d::Mat3f product = m3 * m3_2;
  cout << "elemen setelah operasi m3 * m3_2:" << endl;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) cout << product[i][j] << " ";
    cout << endl;
  }

  cout << "determinant m3 = " << m3.determinant() << endl;

  return 0;
}

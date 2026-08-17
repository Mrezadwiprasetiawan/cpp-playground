#include <string>

#include "matrix.hxx"

int main(int argc, const char **argv) {
  using namespace Linear;
  Mat4d a, b;

  if (argc >= 33) {
    int offset = 1;
    for (int i = 0; i < 16; ++i) {
      a.set_element(i, std::stold(argv[i + offset]));
      b.set_element(i, std::stold(argv[i + offset + 16]));
    }
  } else {
    // default data kalau gak dikasih argumen -- dua matrix 4x4 non-trivial
    // biar hasil perkaliannya keliatan bukan sekadar identitas lagi
    a.set_elements({
        1,
        2,
        0,
        0,
        0,
        1,
        0,
        0,
        0,
        0,
        1,
        3,
        0,
        0,
        0,
        1,
    });
    b.set_elements({
        2,
        0,
        0,
        5,
        0,
        2,
        0,
        -1,
        0,
        0,
        2,
        0,
        0,
        0,
        0,
        1,
    });
  }

  a.debug("Matrix A");
  b.debug("Matrix B");
  Mat4d c = a * b;
  c.debug("Matrix A * B = C");

  Mat<double, 2, 3> A{
      2, 5, 10, 4, 15, 25,
  };
  Mat<double, 2, 3>::solve(A).debug("Solution for 2x+5y=10 and 4x+15y=25 is ");

  return 0;
}
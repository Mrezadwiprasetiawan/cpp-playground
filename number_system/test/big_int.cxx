#include <big_int.hxx>
#include <iostream>

void test_add(const big_int& a, const big_int& b) {
  using namespace std;
  cout << "a + b :" << endl;
  cout << "a = " << a << endl;
  cout << "b = " << b << endl;
  big_int c = a + b;
  cout << "c = " << c << endl;
}

int main() {
  big_int a(100);
  big_int b(-101);
  test_add(a, b);

  a = (1ULL << 63);
  b = 1;
  test_add(a, b);

  return 0;
}

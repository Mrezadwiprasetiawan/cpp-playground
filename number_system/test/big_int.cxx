#include <big_int.hxx>
#include <iostream>

int main() {
  using namespace std;
  big_int a(100);
  big_int b(100);
  big_int c = a + b;
  cout << "for a + b " << endl;
  cout << "a= 100 b=100" << endl;
  cout << c.to_string() << endl;
  a = -100;
  b = 99;
  c = a + b;
  cout << "a = " << a.to_string() << endl;
  cout << "b = " << b.to_string() << endl;
  cout << c.to_string() << endl;

  a = big_int({~0ULL, ~0ULL}, false);
  b = -10;
  c = a + b;
  cout << c.to_string() << endl;
  return 0;
}

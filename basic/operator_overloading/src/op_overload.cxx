#include <op_overload.hxx>

int main() {
  Cat a;
  Dog b;
  using namespace std;
  cout << "kucing berkata ";
  a.berkata();
  cout << "anjing berkata ";
  b.berkata();
  cout << "suara keduanya menjadi :" << endl;
  cout << (a << b) << endl;
  return 0;
}

#include <iostream>
#include <endianness.hxx>

int main(){
  using namespace std;
  cout << "Current Device is " << (is_little_endian()?"Little ":"Big ") << "Endian" << endl;
}

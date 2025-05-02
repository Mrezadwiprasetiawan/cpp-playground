#include <iostream>
#include <utility>  // untuk std::move

class Abstract {
  int a;

 public:
  Abstract(int a) : a(a) {}

  // copy constructor
  Abstract(const Abstract &a) noexcept : a(a.a) {}

  // move constructor
  Abstract(Abstract &&a) noexcept : a(a.a) { a.a = 0; }

  int getA() const { return a; }
};

std::ostream &operator<<(std::ostream &os, const Abstract &abs) {
  return os << abs.getA();
}

int main() {
  std::cout << "isi a dengan 8" << std::endl;
  Abstract a(8);
  std::cout << "move a ke b" << std::endl;
  Abstract b = std::move(a);
  std::cout << "a sekarang berisi " << a << " b sekarang berisi " << b
            << std::endl;
}

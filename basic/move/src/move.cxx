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

std::ostream &operator<<(std::ostream &os, const Abstract &abs) { return os << abs.getA(); }

int main() {
  std::cout << "isi a dengan 8" << std::endl;
  Abstract a(8);
  std::cout << "move a ke b" << std::endl;
  Abstract b = std::move(a);
  std::cout << "a sekarang berisi " << a << " b sekarang berisi " << b << std::endl;
}

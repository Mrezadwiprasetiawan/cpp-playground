/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan

  This project contains various experiments and explorations in C++,
  including topics such as number systems, neural networks, and 
  visualizations of prime number patterns.

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

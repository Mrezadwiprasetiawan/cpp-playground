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

#pragma once
#include <iostream>

class Suara {
 private:
  std::string value;

 public:
  Suara(std::string value) : value(value) {}
  void                 bunyikan() { std::cout << value << std::endl; }
  friend std::ostream& operator<<(std::ostream& a, const Suara& b);
};

class Animal {
 protected:
  std::string suara;

 public:
  Animal() : suara("rrawww") {}
  Animal(std::string s) : suara(s) {}
  void         berkata() { std::cout << suara << std::endl; }
  friend Suara operator<<(const Animal& a, const Animal& b);
};
class Cat : public Animal {
 public:
  Cat() : Animal("meow") {}
};

class Dog : public Animal {
 public:
  Dog() : Animal("guk") {}
};

inline std::ostream& operator<<(std::ostream& a, const Suara& b) { return a << b.value; }

inline Suara operator<<(const Animal& a, const Animal& b) { return Suara(a.suara + " " + b.suara); }

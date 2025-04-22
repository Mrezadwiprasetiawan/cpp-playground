#pragma once
#include <iostream>

class Suara {
 private:
  std::string value;

 public:
  Suara(std::string value) : value(value) {}
  void bunyikan() { std::cout << value << std::endl; }
  friend std::ostream& operator<<(std::ostream& a, const Suara& b);
};

class Animal {
 protected:
  std::string suara;

 public:
  Animal() : suara("rrawww") {}
  Animal(std::string s) : suara(s) {}
  void berkata() { std::cout << suara << std::endl; }
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

inline std::ostream& operator<<(std::ostream& a, const Suara& b) {
  return a << b.value;
}

inline Suara operator<<(const Animal& a, const Animal& b) {
  return Suara(a.suara + " " + b.suara);
}

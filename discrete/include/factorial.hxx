#pragma once
#include <type_traits>

template <typename T>
std::enable_if<std::is_arithmetic_v<T>> factorial(T n) {
  if (n == 1) return 1;
  return n * factorial(n - 1);
}

template <typename T>
T permutation(T n){
  return factorial<T>(n);
}  

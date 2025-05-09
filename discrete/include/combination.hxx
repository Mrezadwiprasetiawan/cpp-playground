#pragma once
#include <type_traits>
#include <factorial.hxx>

template <typename T>
std::enable_if<std::is_arithmetic<T>::value,T>
combination( T n, T k){
  return factorial(n)/(factorial(k) * (factorial(n-k));
}

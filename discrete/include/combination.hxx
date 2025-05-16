#pragma once
#include <type_traits>
#include <factorial.hxx>

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value,T>
combination( T n, T k){
  T res = 1;
  T maxK = k > n/2 ? k : n - k;
  for(T i = maxK + 1; i <= n; ++i) res *= i;
  return res/factorial(n - maxK);
}

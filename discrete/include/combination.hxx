#pragma once
#include <type_traits>
#include <permutation.hxx>

template <typename T>
std::enable_if_t<std::is_arithmetic<T>::value,T>
combination( T n, T k){
  T res = 1;
  T maxK = k > n/2 ? k : n - k;
  for(T i = maxK + 1; i <= n; ++i) res *= i;
  return res/factorial(n - maxK);
}

namespace Discrete {
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
class Combination {
  T n;
  T k;

 public:
  explicit Combination(T n, T k) : n(n), k(k) {}

  T calc() const {
    if (k > n) return 0;
    if (k == 0 || k == n) return 1;
    return combination(n, k);
  }

  template <typename U>
  std::vector<std::vector<U>> choose(const std::vector<U> &sources) const {
    return Permute<U>(sources).choose(n, k);
  }
};
} // namespace Discrete
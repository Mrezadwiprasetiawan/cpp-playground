#include <concepts>
#include <vector>

template <std::integral I>
I int_pow(I base, I exp) {
  if (exp < 0) return 0;
  if (!exp) return 1;
  if (exp & 1) return base * int_pow(base, exp - 1);
  return int_pow(base * base, exp >> 1);
}

template <std::integral I>
std::vector<I> collatz_get_path(I seed) {
  using namespace std;
  vector<I> result;
  while (true) {
    while (!(seed & 1)) seed >>= 1;
    result.push_back(seed);
    if (seed == 1) return result;
    ++seed;
    I v2 = 0, v3 = 0;
    while (!(seed & 1)) {
      seed >>= 1;
      ++v2;
    }
    seed = seed * int_pow<I>(3, v2) - 1;
  }
  return result;
}
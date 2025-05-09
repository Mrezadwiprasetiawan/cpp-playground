#include <cstdint>
#include <type_traits>

template <typename T, typename Ret = uint64_t>
std::enable_if_t<std::is_arithmetic_v<T>>
derangement(T n){
  if (!n) return 1;
  if (n==1) return 0;
  return (n-1)*(derangement(n-1)+derangement(n-2));
}

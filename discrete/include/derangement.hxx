#include <cstdint>
#include <type_traits>
#include <vector>

namespace Discrete {

template <typename T, typename = std::enable_if_t<std::is_integral_v<T>, T>>
class Derangement {
  std::vector<T> caches;

  T pie_calc(T n) {}

  T recursive_calc(T n) {
    if (n == 0) return 1;
    if (n == 1) return 0;
    if (n < caches.size()) return caches[n];
    return (n - 1) * (recursive_calc(n - 1) + recursive_calc(n - 2));
  }

public:
  explicit Derangement(T init_cache) { calc(n); }
  T calc(T n) {}

  template <typename U>
  std::vector<std::vector<U>> dearange(const std::vector<U> &sources) {
    
  }
};
} // namespace Discrete
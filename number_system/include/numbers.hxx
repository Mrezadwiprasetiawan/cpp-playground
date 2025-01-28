#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include <algorithm>

// Template type traits dengan pengecekan ketat
template <typename T, typename Ret = T>
using enable_if_integral = typename std::enable_if<
    std::is_integral<T>::value && !std::is_same<T, bool>::value, Ret>::type;

template <typename T, typename Ret = T>
using enable_if_arithmetic = typename std::enable_if<
    std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, Ret>::type;

// Fungsi eksponensial untuk integer
template <typename T>
enable_if_integral<T, T> power_int(T base, T exponent) {
  if (exponent < 0)
    throw std::invalid_argument("Exponent must be non-negative");

  T result = 1;
  while (exponent > 0) {
    if (exponent % 2 == 1) {
      result *= base;
      if (exponent == 1) break;
    }
    base *= base;
    exponent /= 2;
  }
  return result;
}

template <typename T, typename classRet = enable_if_arithmetic<T>>
class Prime {
 public:
  enum class calc_type_t { NONE, SOE, SIZE };

 private:
  std::vector<T> lastResults;
  T lastT = 0;
  calc_type_t lastType = calc_type_t::NONE;

  std::vector<uint64_t> create_sieve(T limit) {
    if (limit < 3) return {};

    const size_t num_odds = ((limit - 3) >> 1) + 1;
    const size_t array_size = (num_odds + 63) / 64;

    std::vector<uint64_t> sieve(array_size, uint64_t(~0));

    for (T p = 3; p * p <= limit; p += 2) {
      const size_t i = (p - 3) >> 1;
      if (!(sieve[i / 64] & (1ULL << (i % 64)))) continue;

      for (T j = p * p; j <= limit; j += 2 * p) {
        const size_t idx = (j - 3) >> 1;
        sieve[idx / 64] &= ~(1ULL << (idx % 64));
      }
    }
    return sieve;
  }

 public:
  std::vector<T> prime_soe(T limit) {
    if (limit == lastT) return this->lastResults;

    std::vector<T> primes;
    if (limit < 2) return primes;
    primes.push_back(2);

    if (limit < 3) return primes;

    auto sieve = create_sieve(limit);
    const size_t num_odds = ((limit - 3) >> 1) + 1;

    for (size_t i = 0; i < num_odds; ++i)
      if ((sieve[i / 64] & (1ULL << i % 64)) != 0) primes.push_back(3 + 2 * i);

    return primes;
  }

  std::vector<T> from_size(size_t size) {
    if (size == lastT) return this->lastResults;
    std::vector<T> primes;
    if (size == 0) return primes;

    primes.reserve(size);
    primes.push_back(2);

    for (T candidate = 3; primes.size() < size; candidate += 2) {
      bool is_prime = true;
      const T sqrt_candidate = static_cast<T>(std::sqrt(candidate)) + 1;

      for (const auto &prime : primes) {
        if (prime > sqrt_candidate) break;
        if (candidate % prime == 0) {
          is_prime = false;
          break;
        }
      }

      if (is_prime) primes.push_back(candidate);
    }

    return primes;
  }
};

class Fibonacci {
 private:
  std::vector<std::vector<uint64_t>> values;
  size_t lastLimit = 0;
  std::vector<std::string> values_str;
  const std::string TWO_POW_64 = "18446744073709551616";

  std::string multiplyStrings(const std::string &a, const std::string &b) {
    if (a == "0" || b == "0") return "0";

    int m = a.size(), n = b.size();
    std::vector<int> res(m + n, 0);

    for (int i = m - 1; i >= 0; i--) {
      for (int j = n - 1; j >= 0; j--) {
        int mul = (a[i] - '0') * (b[j] - '0');
        int p1 = i + j, p2 = i + j + 1;
        int sum = mul + res[p2];

        res[p2] = sum % 10;
        res[p1] += sum / 10;
      }
    }

    std::string result;
    for (int num : res) {
      if (!(result.empty() && num == 0)) {
        result.push_back(num + '0');
      }
    }
    return result.empty() ? "0" : result;
  }

  std::string multiplyBy2To64(const std::string &numStr) {
    return multiplyStrings(numStr, TWO_POW_64);
  }

  std::string addUint64ToString(const std::string &numStr, uint64_t add) {
    std::string result;
    int carry = 0;
    int i = numStr.size() - 1;
    uint64_t add_val = add;

    while (i >= 0 || add_val > 0 || carry > 0) {
      int sum = carry;

      if (i >= 0) {
        sum += numStr[i--] - '0';
      }

      if (add_val > 0) {
        sum += add_val % 10;
        add_val /= 10;
      }

      carry = sum / 10;
      result.push_back(sum % 10 + '0');
    }

    std::reverse(result.begin(), result.end());
    size_t start = result.find_first_not_of('0');
    return (start != std::string::npos) ? result.substr(start) : "0";
  }

  std::vector<uint64_t> add64_ext(const std::vector<uint64_t>& a, const std::vector<uint64_t>& b) {
    std::vector<uint64_t> res;
    size_t max_size = std::max(a.size(), b.size());
    std::vector<uint64_t> a_copy = a;
    std::vector<uint64_t> b_copy = b;
    a_copy.resize(max_size, 0);
    b_copy.resize(max_size, 0);

    uint64_t carry = 0;

    for (size_t i = 0; i < max_size; ++i) {
      uint64_t sum_limb = a_copy[i] + b_copy[i];
      uint64_t carry1 = sum_limb < a_copy[i] ? 1 : 0;

      sum_limb += carry;
      uint64_t carry2 = sum_limb < carry ? 1 : 0;

      res.push_back(sum_limb);
      carry = carry1 + carry2;
    }

    if (carry) {
      res.push_back(carry);
    }

    while (!res.empty() && res.back() == 0) {
      res.pop_back();
    }

    return res.empty() ? std::vector<uint64_t>{0} : res;
  }

  std::vector<std::string> decode() {
    std::vector<std::string> res;
    for (const auto &fbnc : values) {
      std::string current = "0";
      for (auto it = fbnc.rbegin(); it != fbnc.rend(); ++it) {
        current = multiplyBy2To64(current);
        current = addUint64ToString(current, *it);
      }
      res.push_back(current);
    }
    return res;
  }

  void generate(size_t limit) {
    if (limit < lastLimit) {
      values.resize(limit);
      return;
    }

    if (values.empty()) {
      values.push_back({0}); // F0
      values.push_back({1}); // F1
    }

    size_t currentSize = values.size();
    if (limit <= currentSize) {
      lastLimit = limit;
      values_str = decode();
      return;
    }

    values.resize(limit);

    for (size_t i = currentSize; i < limit; ++i) {
      if (i < 2) continue;
      values[i] = add64_ext(values[i-1], values[i-2]);
    }

    lastLimit = limit;
    values_str = decode();
  }

 public:
  std::vector<std::string> get_all(size_t limit) {
    if (limit == lastLimit) return values_str;
    generate(limit);
    return values_str;
  }

  std::string get_index(size_t index) {
    if (index >= lastLimit) {
      generate(index + 1);
    }
    return get_all(index + 1)[index];
  }
};

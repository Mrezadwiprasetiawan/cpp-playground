#pragma once

#include <type_traits>
#include <vector>
#include <cstdint>
#include <thread>

template <typename T, typename Ret = T>
using enable_if_arithmetic = typename std::enable_if<
    std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, Ret>::type;

template <typename T, enable_if_arithmetic<T> = 0>
class Prime {
 private:
  std::vector<T> lastResults;
  T lastLimit = 0;
  T lastSize = 0;
  inline static int maxThread = std::thread::hardware_concurrency();

  void main_sieve(std::vector<uint64_t> &sieve, T limit, int offset) {
    for (T p = 3 + offset * 2; p * p <= limit; p += 2 * maxThread) {
      const size_t i = (p - 3) >> 1;
      if (!(sieve[i >> 6] & (1ULL << (i & 63)))) continue;
      for (T j = p * p; j <= limit; j += 2 * p) {
        const size_t idx = (j - 3) >> 1;
        sieve[idx >> 6] &= ~(1ULL << (idx & 63));
      }
    }
  }

  std::vector<uint64_t> create_sieve(T limit) {
    if (limit < 3) return {};
    const size_t num_odds = ((limit - 3) >> 1) + 1;
    const size_t array_size = (num_odds + 63) >> 6;
    std::vector<uint64_t> sieve(array_size, uint64_t(~0));
    std::vector<std::thread> threads;
    for (int i = 1; i < maxThread; ++i)
      threads.emplace_back(
          [this, &sieve, limit, i]() { main_sieve(sieve, limit, i); });
    main_sieve(sieve, limit, 0);
    for (auto &t : threads) t.join();
    return sieve;
  }

  T estimate_limit_from_size(size_t size) {
    if (size < 6) return (1 << 4) - 1;
    double n = static_cast<double>(size);
    return static_cast<T>(n * (std::log(n) + std::log(std::log(n)))) + 10;
  }

 public:
  std::vector<T> from_size(size_t size) {
    if (size <= lastSize) {
      this->lastResults.resize(size);
      return this->lastResults;
    }
    if (size == 0) return {};
    std::vector<T> primes;
    primes.push_back(2);
    if (size == 1) return primes;
    T limit = estimate_limit_from_size(size);
    lastLimit = limit;
    auto sieve = create_sieve(limit);
    const size_t num_odds = ((limit - 3) >> 1) + 1;

    for (size_t i = 0; i < num_odds && primes.size() < size; ++i)
      if (sieve[i >> 6] & (1ULL << (i & 63))) primes.emplace_back(3 + 2 * i);

    lastResults = primes;
    return primes;
  }

  std::vector<T> from_range_limit(T limit) {
    if (limit == lastLimit) return this->lastResults;
    std::vector<T> primes;
    if (limit < 2) return primes;
    primes.push_back(2);
    if (limit < 3) return primes;
    lastLimit = limit;
    auto sieve = create_sieve(limit);
    const size_t num_odds = ((limit - 3) >> 1) + 1;

    for (size_t i = 0; i < num_odds; ++i)
      if (sieve[i >> 6] & (1ULL << (i & 63))) primes.emplace_back(3 + 2 * i);

    lastResults = primes;
    return primes;
  }

  bool is_prime(T n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if ((n & 1) == 0) return false;
    T sqrt_n = static_cast<T>(std::sqrt(n));
    auto sieve = from_range_limit(sqrt_n);
    for (T p : sieve)
      if (n % p == 0) return false;
    return true;
  }
  static int max_thread() { return Prime::maxThread; }
};

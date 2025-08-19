/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <cmath>
#include <concepts>
#include <cstdint>
#include <thread>
#include <type_traits>
#include <vector>

namespace Discrete {
template <typename T>
requires(std::integral<T> || std::floating_point<T> && !std::is_same_v<bool, T>) class Prime {
 private:
  std::vector<T>    lastResults;
  T                 lastLimit = 0;
  T                 lastSize  = 0;
  inline static int maxThread = std::thread::hardware_concurrency();

  void main_sieve(std::vector<uint64_t> &sieve, T limit, int offset) noexcept {
    for (T p = 3 + offset * 2; p * p <= limit; p += 2 * maxThread) {
      const size_t i = (p - 3) >> 1;
      if (!(sieve[i >> 6] & (1ULL << (i & 63)))) continue;
      for (T j = p * p; j <= limit; j += 2 * p) {
        const size_t idx  = (j - 3) >> 1;
        sieve[idx >> 6]  &= ~(1ULL << (idx & 63));
      }
    }
  }

  std::vector<uint64_t> create_sieve(T limit) noexcept {
    if (limit < 3) return {};
    const size_t             numOdds   = ((limit - 3) >> 1) + 1;
    const size_t             arraySize = (numOdds + 63) >> 6;
    std::vector<uint64_t>    sieve(arraySize, uint64_t(~0));
    std::vector<std::thread> threads;
    for (int i = 1; i < maxThread; ++i) threads.emplace_back([this, &sieve, limit, i]() { main_sieve(sieve, limit, i); });
    main_sieve(sieve, limit, 0);
    for (auto &t : threads) t.join();
    return sieve;
  }

  T estimate_limit_from_size(size_t size) noexcept {
    if (size < 6) return (1 << 4) - 1;
    double n = static_cast<double>(size);
    return static_cast<T>(n * (std::log(n) + std::log(std::log(n)))) + 10;
  }

 public:
  explicit Prime() noexcept {}
  std::vector<T> from_size(size_t size) noexcept {
    if (size <= lastSize) {
      if (size == lastSize) return this->lastResults;
      return std::vector<T>(this->lastResults.begin(), this->lastResults.begin() + size);
    }
    if (!size) return {};
    std::vector<T> primes;
    primes.push_back(2);
    if (size == 1) return primes;
    T limit              = estimate_limit_from_size(size);
    lastLimit            = limit;
    auto         sieve   = create_sieve(limit);
    const size_t numOdds = ((limit - 3) >> 1) + 1;
    for (size_t i = 0; i < numOdds && primes.size() < size; ++i)
      if (sieve[i >> 6] & (1ULL << (i & 63))) primes.emplace_back(3 + 2 * i);
    lastResults = primes;
    lastSize    = size;
    return primes;
  }

  std::vector<T> from_range_limit(T limit) noexcept {
    std::vector<T> primes;
    if (limit < 2) return primes;
    primes.push_back(2);
    if (limit < 3) return primes;
    if (limit <= lastLimit) {
      if (limit == lastLimit) return this->lastResults;

      // estimasi end awal
      size_t end = static_cast<size_t>(limit / std::log(limit)) + 1;
      if (end > lastResults.size()) end = lastResults.size();
      while (end < lastResults.size() && lastResults[end] <= limit) ++end;
      return std::vector<T>(this->lastResults.begin(), this->lastResults.begin() + end);
    }
    lastLimit            = limit;
    auto         sieve   = create_sieve(limit);
    const size_t numOdds = ((limit - 3) >> 1) + 1;
    for (size_t i = 0; i < numOdds; ++i)
      if (sieve[i >> 6] & (1ULL << (i & 63))) primes.emplace_back(3 + 2 * i);
    lastResults = primes;
    return primes;
  }

  bool is_prime(T n) noexcept {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (!(n & 1)) return false;
    T    sqrtN = static_cast<T>(std::sqrt(n));
    auto sieve = from_range_limit(sqrtN);
    for (T p : sieve)
      if (!(n % p)) return false;
    return true;
  }

  static int max_thread() noexcept { return Prime::maxThread; }

  void clear_cache() noexcept {
    //to ensure that the stack is freed properly, we use swap() of some temporary vector which will be destroyed after the method is executed
    std::vector<T>().swap(lastResults);
    
    
    lastSize  = 0;
    lastLimit = 0;
  }
};
}  // namespace Discrete

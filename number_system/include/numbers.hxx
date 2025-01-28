#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <type_traits>
#include <vector>

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

// Setiap bit dalam uint64_t merepresentasikan bilangan ganjil >2
// Bit 0: 3, Bit 1: 5, Bit 2: 7, dst...
template <typename T>
std::vector<uint64_t> create_sieve(T limit) {
  if (limit < 3) return {};

  // Hitung jumlah bilangan ganjil yang perlu direpresentasikan
  const size_t num_odds = ((limit - 3) >> 1) + 1;
  const size_t array_size = (num_odds + 63) / 64;

  std::vector<uint64_t> sieve(array_size, uint64_t(~0));

  // 0 = komposit, 1 = prima (inisialisasi semua sebagai prima)
  for (T p = 3; p * p <= limit; p += 2) {
    const size_t i = (p - 3) >> 1;
    if (!(sieve[i / 64] & (1ULL << (i % 64)))) continue;

    // Mark kelipatan p mulai dari p^2
    for (T j = p * p; j <= limit; j += 2 * p) {
      const size_t idx = (j - 3) >> 1;
      sieve[idx / 64] &= ~(1ULL << (idx % 64));
    }
  }
  return sieve;
}

template <typename T>
bool check_prime(const T *sieve, size_t index) {
  const size_t bucket = index / 64;
  const size_t bit = index % 64;
  return (sieve[bucket] & (1ULL << bit)) != 0;
}

template <typename T>
std::vector<T> prime_soe(T limit) {
  std::vector<T> primes;
  if (limit < 2) return primes;
  primes.push_back(2);

  if (limit < 3) return primes;

  auto sieve = create_sieve(limit);
  const size_t num_odds = ((limit - 3) >> 1) + 1;

  for (size_t i = 0; i < num_odds; ++i)
    if (check_prime(sieve.data(), i)) primes.push_back(3 + 2 * i);

  return primes;
}

// Fungsi pencarian prima dengan trial division yang dioptimasi
template <typename T>
enable_if_integral<T, std::vector<T>> prime_size(size_t size) {
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

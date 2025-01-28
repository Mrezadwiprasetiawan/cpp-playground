#include <cmath>
#include <cstddef>
#include <cstdint>
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

template <typename T, typename classRet = enable_if_arithmetic<T>>
class Prime {
 public:
  enum class calc_type_t { NONE, SOE, SIZE };

 private:
  // Setiap bit dalam uint64_t merepresentasikan bilangan ganjil >2
  // Bit 0: 3, Bit 1: 5, Bit 2: 7, dst...
  std::vector<T> lastResults;
  T lastT = 0;
  calc_type_t lastType = calc_type_t::NONE;

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

 public:
  // sieve of eratosthenes
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

  // Fungsi pencarian prima dengan trial division yang dioptimasi
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

  // Prekomputasi nilai 2^64 dalam string
  const std::string TWO_POW_64 = "18446744073709551616";

  // Helper untuk perkalian string angka (optimized schoolbook)
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

  // Helper untuk mengalikan dengan 2^64 (optimized)
  std::string multiplyBy2To64(const std::string &numStr) {
    return multiplyStrings(numStr, TWO_POW_64);
  }

  // Helper untuk penambahan uint64 ke string (optimized)
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

    // Hapus leading zero
    size_t start = result.find_first_not_of('0');
    return (start != std::string::npos) ? result.substr(start) : "0";
  }
  
  // Fungsi untuk menambahkan dua uint64_t dengan carry
  std::pair<uint64_t, uint64_t> add64(uint64_t a, uint64_t b) {
    uint64_t sum = a + b;
    uint64_t carry = sum < a ? 1 : 0;
    return {carry, sum};
  }

  // Fungsi untuk menambahkan dua vektor uint64_t
  std::vector<uint64_t> add64_ext(std::vector<uint64_t> a,
                                  std::vector<uint64_t> b) {
    std::vector<uint64_t> res;
    bool same = a.size() == b.size();
    size_t min = a.size() < b.size() ? a.size() : b.size();
    uint64_t carry = 0;
    for (size_t i = 0; i < min; ++i) {
      std::pair<uint64_t, uint64_t> currentRes = add64(a[i], b[i]);
      carry = currentRes.first ? currentRes.first : 0;
      res.push_back(carry + currentRes.second);
    }
    if (!same) {
      bool gtA = a.size() > b.size() ? a.size() : b.size();
      for (size_t i = gtA ? b.size() : a.size(); i < gtA ? a.size() : b.size();
           ++i)
        res.push_back(gtA ? a[i] : b[i]);
    }
    return res;
  }

  // Fungsi untuk decode vektor<uint64_t> ke string basis 10
  std::vector<std::string> decode() {
    std::vector<std::string> res;
    for (const auto &fbnc : values) {
      std::string current = "0";

      // Iterasi dari MSB ke LSB (reverse iterator)
      for (auto it = fbnc.rbegin(); it != fbnc.rend(); ++it) {
        current = multiplyBy2To64(current);
        current = addUint64ToString(current, *it);
      }

      res.push_back(current);
    }
    return res;
  }

  public:
  std::vector<std::string> get_all(size_t limit){
    if(limit==lastLimit) return values_str;
    return decode();
  }
  std::string get_index(size_t index){
    return get_all(index+1)[index];
  }
};

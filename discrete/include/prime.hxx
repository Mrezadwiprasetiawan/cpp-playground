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

#include <bit.hxx>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <heap.hxx>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#if !defined(__cpp_lib_endian)
namespace std {
enum class endian {
  little = 0,
  big    = 1,
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && defined(__ORDER_BIG_ENDIAN__)
  native = (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) ? little : big
#elif defined(_WIN32)
  native = little
#elif defined(__APPLE__)
  // Apple platforms are little-endian on Intel and ARM64
  native = little
#elif defined(__MACH__) || defined(__linux__)
#ifdef __BIG_ENDIAN__
  native = big
#else
  native = little
#endif
#else
  // fallback default
  native = little
#endif
};
}  // namespace std
#endif

namespace Discrete {
template <typename T>
requires(std::integral<T> || std::floating_point<T> && !std::is_same_v<bool, T>) class Prime {
 private:
  std::vector<T>    lastResults;
  T                 lastLimit = 0;
  T                 lastSize  = 0;
  inline static int maxThread = std::thread::hardware_concurrency();

  void main_sieve(std::vector<uint64_t> &sieve, T limit, int tid) noexcept {
    for (T p = 3; p * p <= limit; p += 2) {
      const size_t i = (p - 3) >> 1;
      if (!(sieve[i >> 6] & (1ULL << (i & 63)))) continue;
      for (T j = p * p + 2 * p * tid; j <= limit; j += 2 * p * maxThread) {
        const size_t idx  = (j - 3) >> 1;
        sieve[idx >> 6]  &= ~(1ULL << (idx & 63));
      }
    }
  }

  std::vector<uint64_t> create_sieve(T limit) {
    using namespace std;
    if (limit < 3) return {};
    const size_t numOdds   = ((limit - 3) >> 1) + 1;
    const size_t arraySize = (numOdds + 63) >> 6;
    const size_t heapSize  = get_available_heap();
    if ((arraySize + limit / log(limit) + 1) * 8 > heapSize)
      throw std::runtime_error("not enough heap to do bitsieve operation, Heap = " + to_string(heapSize));
    vector<uint64_t>    sieve(arraySize, uint64_t(~0));
    vector<std::thread> threads;
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
  explicit Prime() noexcept {}
  explicit Prime(const Prime &)    = delete;
  Prime &operator=(const Prime &)  = delete;
  explicit Prime(const Prime &&)   = delete;
  Prime &operator=(const Prime &&) = delete;

 public:
  static Prime &instance() {
    static Prime inst;
    return inst;
  }
  std::vector<T> from_size(size_t size, bool storageHelp = false) {
    using namespace std;
    if (size <= lastSize) {
      if (size == lastSize) return this->lastResults;
      return vector<T>(this->lastResults.begin(), this->lastResults.begin() + size);
    }
    if (!size) return {};
    T limit  = estimate_limit_from_size(size);
    lastSize = size;
    return from_range_limit(limit, storageHelp);
  }

  std::vector<T> from_range_limit(T limit, bool storageHelp = false) {
    using namespace std;
    vector<T> primes;
    if (limit < 2) return primes;
    primes.push_back(2);
    if (limit < 3) return primes;
    if (limit <= lastLimit) {
      if (limit == lastLimit) return this->lastResults;
      // estimasi end awal
      size_t end = static_cast<size_t>(limit / log(limit)) + 1;
      if (end > lastResults.size()) end = lastResults.size();
      while (end < lastResults.size() && lastResults[end] <= limit) ++end;
      return vector<T>(this->lastResults.begin(), this->lastResults.begin() + end);
    }
    lastLimit            = limit;
    auto         sieve   = create_sieve(limit);  // pass the exception to caller if exist
    const size_t numOdds = ((limit - 3) >> 1) + 1;
    for (size_t i = 0; i < numOdds; ++i)
      if (sieve[i >> 6] & (1ULL << (i & 63))) primes.emplace_back(3 + 2 * i);
    lastResults = primes;
    return primes;
  }

  bool is_prime(T n) {
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
    // to ensure that the heap is freed properly, we use swap() of some temporary vector which will be destroyed after the method is executed
    std::vector<T>().swap(lastResults);
    lastSize  = 0;
    lastLimit = 0;
  }

  bool write_sieve(const std::string &filename) {
    using namespace std;
    ofstream ofs(filename, ios::binary);
    if (!ofs.is_open()) {
      cerr << "[write_sieve] Error: failed to open file '" << filename << "' for writing.\n";
      return false;
    }
    char mark = std::endian::native == std::endian::little ? 1 : 0;
    ofs.write(&mark, 1);
    if (!ofs) {
      cerr << "[write_sieve] Error: failed to write endian mark to file '" << filename << "'.\n";
      return false;
    }
    const size_t count = lastResults.size();
    const size_t bytes = count * sizeof(T);
    if (!bytes) return true;  // nothing to write
    try {
      ofs.seekp(static_cast<std::streamoff>(1 + bytes - 1));
      char zero = 0;
      ofs.write(&zero, 1);
      ofs.flush();
    } catch (...) {
      cerr << "[write_sieve] Exception while preparing file '" << filename << "'.\n";
      return false;
    }
    ofs.seekp(1);
    ofs.write(reinterpret_cast<const char *>(lastResults.data()), static_cast<std::streamsize>(bytes));
    if (!ofs) {
      cerr << "[write_sieve] Error: write failed.\n";
      return false;
    }
    ofs.close();
    return true;
  }

  bool load_sieve(const std::string &filename) {
    using namespace std;
    ifstream ifs(filename, ios::binary | ios::ate);
    if (!ifs.is_open()) {
      cerr << "[load_sieve] Failed to open file: " << filename << endl;
      return false;
    }

    streamsize fsize = ifs.tellg();
    if (fsize < 1) {
      cerr << "[load_sieve] File too small or tellg failed: " << filename << endl;
      return false;
    }

    ifs.seekg(0, ios::beg);
    char fileEndian = 0;
    ifs.read(&fileEndian, 1);
    if (!ifs) {
      cerr << "[load_sieve] Failed to read endian mark from: " << filename << endl;
      return false;
    }
    char         nativeEndian = std::endian::native == std::endian::little ? 1 : 0;
    bool         needSwap     = fileEndian != nativeEndian;
    const size_t total_bytes  = static_cast<size_t>(fsize) - 1;
    if (total_bytes % sizeof(T) != 0) {
      cerr << "[load_sieve] Invalid file size (not multiple of sizeof(T)): " << fsize << " bytes" << endl;
      return false;
    }
    const size_t count = total_bytes / sizeof(T);
    lastResults.resize(count);
    ifs.seekg(1, ios::beg);
    ifs.read(reinterpret_cast<char *>(lastResults.data()), static_cast<std::streamsize>(total_bytes));
    if (!ifs) {
      cerr << "[load_sieve] Read failed (" << ifs.gcount() << ")\n";
      return false;
    }
    ifs.close();
    if (needSwap)
      for (size_t i = 0; i < count; ++i) lastResults[i] = bswap64(lastResults[i]);
    cout << "[load_sieve] Successfully read " << count << " elements from file: " << filename << (needSwap ? " (byte-swapped due to endian mismatch)" : "")
         << endl;
    return true;
  }
};
}  // namespace Discrete

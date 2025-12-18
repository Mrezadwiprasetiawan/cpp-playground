#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "uint512_t.hxx"

class SHA3_512 {
 private:
  static constexpr size_t KECCAKF_ROUNDS = 24;
  static constexpr uint64_t keccakf_rndc[24] = {
      0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL, 0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
      0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL, 0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
      0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL, 0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
      0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL, 0x8000000000008080ULL, 0x0000000000008001ULL, 0x8000000080008008ULL};

  static constexpr uint8_t keccakf_rotc[24] = {1, 3, 6, 10, 15, 21, 28, 36, 45, 55, 2, 14, 27, 41, 56, 8, 25, 43, 62, 18, 39, 61, 20, 44};

  static constexpr uint8_t keccakf_piln[24] = {10, 7, 11, 17, 18, 3, 5, 16, 8, 21, 24, 4, 15, 23, 19, 13, 12, 2, 20, 14, 22, 9, 6, 1};

  static inline uint64_t rol64(uint64_t x, unsigned s) { return (x << s) | (x >> (64 - s)); }

  static void keccakf(uint64_t* st) {
    uint64_t t, bc[5];
    for (size_t round = 0; round < KECCAKF_ROUNDS; ++round) {
      for (int i = 0; i < 5; ++i) bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
      for (int i = 0; i < 5; ++i) {
        t = bc[(i + 4) % 5] ^ rol64(bc[(i + 1) % 5], 1);
        for (int j = 0; j < 25; j += 5) st[j + i] ^= t;
      }
      t = st[1];
      for (int i = 0; i < 24; ++i) {
        int j = keccakf_piln[i];
        bc[0] = st[j];
        st[j] = rol64(t, keccakf_rotc[i]);
        t = bc[0];
      }
      for (int j = 0; j < 25; j += 5) {
        for (int i = 0; i < 5; ++i) bc[i] = st[j + i];
        st[j + 0] = bc[0] ^ ((~bc[1]) & bc[2]);
        st[j + 1] = bc[1] ^ ((~bc[2]) & bc[3]);
        st[j + 2] = bc[2] ^ ((~bc[3]) & bc[4]);
        st[j + 3] = bc[3] ^ ((~bc[4]) & bc[0]);
        st[j + 4] = bc[4] ^ ((~bc[0]) & bc[1]);
      }
      st[0] ^= keccakf_rndc[round];
    }
  }

  static inline bool is_little_endian() {
    uint16_t x = 1;
    return *reinterpret_cast<uint8_t*>(&x) == 1;
  }

  static inline uint64_t swap64(uint64_t x) {
    return ((x & 0x00000000000000FFULL) << 56) | ((x & 0x000000000000FF00ULL) << 40) | ((x & 0x0000000000FF0000ULL) << 24) |
           ((x & 0x00000000FF000000ULL) << 8) | ((x & 0x000000FF00000000ULL) >> 8) | ((x & 0x0000FF0000000000ULL) >> 24) | ((x & 0x00FF000000000000ULL) >> 40) |
           ((x & 0xFF00000000000000ULL) >> 56);
  }

  static void update(const uint8_t* data, size_t len, uint64_t (&st)[25], std::vector<uint8_t>& buf) {
    while (len--) {
      buf.push_back(*data++);
      if (buf.size() == 72) {
        for (size_t i = 0; i < 9; ++i) {
          uint64_t t = 0;
          for (size_t j = 0; j < 8; ++j) t |= ((uint64_t)buf[i * 8 + j]) << (8 * j);
          st[i] ^= t;
        }
        keccakf(st);
        buf.clear();
      }
    }
  }

  static void finalize(uint64_t (&st)[25], std::vector<uint8_t>& buf) {
    buf.push_back(0x06);
    while (buf.size() < 72) buf.push_back(0x00);
    buf[71] |= 0x80;
    for (size_t i = 0; i < 9; ++i) {
      uint64_t t = 0;
      for (size_t j = 0; j < 8; ++j) t |= ((uint64_t)buf[i * 8 + j]) << (8 * j);
      st[i] ^= t;
    }
    keccakf(st);
  }

 public:
  static uint512_t hash(const std::string& s) {
    using namespace std;
    uint64_t st[25]{};
    vector<uint8_t> buf;
    update(reinterpret_cast<const uint8_t*>(s.data()), s.size(), st, buf);
    finalize(st, buf);

    array<uint8_t, 64> out{};
    bool little = is_little_endian();
    for (size_t i = 0; i < 8; ++i) {
      uint64_t lane = st[i];
      if (little) lane = swap64(lane);
      for (int j = 0; j < 8; ++j) out[i * 8 + j] = (lane >> (8 * (7 - j))) & 0xFF;
    }
    uint512_t val;
    for (int i = 0; i < 64; ++i) val.dataBE[i] = out[i];
    return val;
  }

  static std::string hash_hex(const std::string& s) {
    using namespace std;
    uint64_t st[25]{};
    vector<uint8_t> buf;
    update(reinterpret_cast<const uint8_t*>(s.data()), s.size(), st, buf);
    finalize(st, buf);

    ostringstream oss;
    bool little = is_little_endian();
    for (size_t i = 0; i < 8; ++i) {
      uint64_t lane = st[i];
      if (little) lane = swap64(lane);
      for (int j = 0; j < 8; ++j) {
        uint8_t b = (lane >> (8 * (7 - j))) & 0xFF;
        oss << hex << setw(2) << setfill('0') << (int)b;
      }
    }
    return oss.str();
  }
};

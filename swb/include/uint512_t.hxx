#pragma once
#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>

class uint512_t {
  uint8_t dataBE[64]{};
  friend class SHA3_512;

  static void clear_buf(uint8_t* buf) { std::memset(buf, 0, 64); }

  static int cmp_buf(const uint8_t* a, const uint8_t* b) {
    for (int i = 0; i < 64; ++i)
      if (a[i] != b[i]) return a[i] < b[i] ? -1 : 1;
    return 0;
  }

  static void add_buf_inplace(uint8_t* a, const uint8_t* b) {
    int carry = 0;
    for (int i = 63; i >= 0; --i) {
      int sum = a[i] + b[i] + carry;
      a[i] = static_cast<uint8_t>(sum & 0xFF);
      carry = sum >> 8;
    }
  }

  static void sub_buf_inplace(uint8_t* a, const uint8_t* b) {
    int borrow = 0;
    for (int i = 63; i >= 0; --i) {
      int diff = static_cast<int>(a[i]) - b[i] - borrow;
      if (diff < 0) {
        diff += 256;
        borrow = 1;
      } else borrow = 0;
      a[i] = static_cast<uint8_t>(diff);
    }
  }

  static void lshift1_buf(uint8_t* a) {
    uint8_t carry = 0;
    for (int i = 63; i >= 0; --i) {
      uint8_t new_carry = (a[i] & 0x80) ? 1 : 0;
      a[i] = static_cast<uint8_t>((a[i] << 1) | carry);
      carry = new_carry;
    }
  }

  static void rshift1_buf(uint8_t* a) {
    uint8_t carry = 0;
    for (int i = 0; i < 64; ++i) {
      uint8_t new_carry = (a[i] & 1) ? 0x80 : 0;
      a[i] = static_cast<uint8_t>((a[i] >> 1) | carry);
      carry = new_carry;
    }
  }

  static uint8_t get_bit_from_buf(const uint8_t* a, int idx) {
    int byte = idx / 8;
    int bit = 7 - (idx % 8);
    return (a[byte] >> bit) & 1;
  }

  static void set_bit_in_buf(uint8_t* a, int idx) {
    int byte = idx / 8;
    int bit = 7 - (idx % 8);
    a[byte] |= static_cast<uint8_t>(1u << bit);
  }

  static void clear_bit_in_buf(uint8_t* a, int idx) {
    int byte = idx / 8;
    int bit = 7 - (idx % 8);
    a[byte] &= static_cast<uint8_t>(~(1u << bit));
  }

  bool is_zero() const {
    for (uint8_t b : dataBE)
      if (b) return false;
    return true;
  }

  uint8_t div_small_inplace_buf(uint8_t* a, uint8_t v) const {
    uint16_t rem = 0;
    for (int i = 0; i < 64; ++i) {
      uint16_t cur = (rem << 8) | a[i];
      a[i] = static_cast<uint8_t>(cur / v);
      rem = cur % v;
    }
    return static_cast<uint8_t>(rem);
  }

 public:
  constexpr uint512_t() = default;

  explicit constexpr uint512_t(uint8_t v) { dataBE[63] = v; }
  explicit constexpr uint512_t(uint16_t v) {
    dataBE[63] = v & 0xFF;
    dataBE[62] = (v >> 8) & 0xFF;
  }
  explicit constexpr uint512_t(uint32_t v) {
    for (int i = 0; i < 4; ++i) dataBE[63 - i] = (v >> (8 * i)) & 0xFF;
  }
  explicit constexpr uint512_t(uint64_t v) {
    for (int i = 0; i < 8; ++i) dataBE[63 - i] = (v >> (8 * i)) & 0xFF;
  }
  explicit constexpr uint512_t(int8_t v) : uint512_t(static_cast<uint8_t>(v < 0 ? 0 : v)) {}
  explicit constexpr uint512_t(int16_t v) : uint512_t(static_cast<uint16_t>(v < 0 ? 0 : v)) {}
  explicit constexpr uint512_t(int32_t v) : uint512_t(static_cast<uint32_t>(v < 0 ? 0 : v)) {}
  explicit constexpr uint512_t(int64_t v) : uint512_t(static_cast<uint64_t>(v < 0 ? 0 : v)) {}

  uint512_t operator+(const uint512_t& b) const {
    uint512_t r;
    std::memcpy(r.dataBE, dataBE, 64);
    add_buf_inplace(r.dataBE, b.dataBE);
    return r;
  }

  uint512_t operator-(const uint512_t& b) const {
    uint512_t r;
    std::memcpy(r.dataBE, dataBE, 64);
    sub_buf_inplace(r.dataBE, b.dataBE);
    return r;
  }

  uint512_t operator*(const uint512_t& b) const {
    uint8_t tmp[64]{};
    for (int i = 63; i >= 0; --i) {
      uint16_t carry = 0;
      for (int j = 63; j >= 0; --j) {
        int k = i + j - 63;
        if (k < 0) break;
        uint32_t prod = tmp[k] + static_cast<uint32_t>(dataBE[i]) * b.dataBE[j] + carry;
        tmp[k] = static_cast<uint8_t>(prod & 0xFF);
        carry = static_cast<uint16_t>(prod >> 8);
      }
    }
    uint512_t r;
    std::memcpy(r.dataBE, tmp, 64);
    return r;
  }

  uint512_t operator<<(size_t shift) const {
    if (shift >= 512) return uint512_t();
    uint8_t tmp[64]{};
    size_t byteShift = shift / 8;
    uint8_t bitShift = shift % 8;
    for (int i = 0; i < 64; ++i) {
      int dst = i - static_cast<int>(byteShift);
      if (dst < 0 || dst >= 64) continue;
      uint16_t val = static_cast<uint16_t>(dataBE[i]) << bitShift;
      tmp[dst] = static_cast<uint8_t>(val & 0xFF);
      if (dst - 1 >= 0 && bitShift) tmp[dst - 1] |= static_cast<uint8_t>(val >> 8);
    }
    uint512_t r;
    std::memcpy(r.dataBE, tmp, 64);
    return r;
  }

  uint512_t operator>>(size_t shift) const {
    if (shift >= 512) return uint512_t();
    uint8_t tmp[64]{};
    size_t byteShift = shift / 8;
    uint8_t bitShift = shift % 8;
    for (int i = 63; i >= 0; --i) {
      int dst = i + static_cast<int>(byteShift);
      if (dst < 0 || dst >= 64) continue;
      uint16_t val = static_cast<uint16_t>(dataBE[i]) >> bitShift;
      tmp[dst] = static_cast<uint8_t>(val & 0xFF);
      if (dst + 1 < 64 && bitShift) tmp[dst + 1] |= static_cast<uint8_t>((dataBE[i] << (8 - bitShift)) & 0xFF);
    }
    uint512_t r;
    std::memcpy(r.dataBE, tmp, 64);
    return r;
  }

  uint512_t operator*(uint8_t small) const {
    uint8_t tmp[64]{};
    uint16_t carry = 0;
    for (int i = 63; i >= 0; --i) {
      uint32_t prod = static_cast<uint32_t>(dataBE[i]) * small + carry;
      tmp[i] = static_cast<uint8_t>(prod & 0xFF);
      carry = static_cast<uint16_t>(prod >> 8);
    }
    uint512_t r;
    std::memcpy(r.dataBE, tmp, 64);
    return r;
  }

  uint512_t operator/(const uint512_t& divisor) const {
    uint8_t dividend[64];
    std::memcpy(dividend, dataBE, 64);
    if (cmp_buf(dividend, divisor.dataBE) < 0) return uint512_t();
    uint8_t quotient[64]{};
    uint8_t remainder[64]{};
    for (int i = 0; i < 512; ++i) {
      lshift1_buf(remainder);
      if (get_bit_from_buf(dividend, i)) remainder[63] |= 1;
      if (cmp_buf(remainder, divisor.dataBE) >= 0) {
        sub_buf_inplace(remainder, divisor.dataBE);
        set_bit_in_buf(quotient, i);
      }
    }
    uint512_t q;
    std::memcpy(q.dataBE, quotient, 64);
    return q;
  }

  uint512_t operator%(const uint512_t& divisor) const {
    uint8_t dividend[64];
    std::memcpy(dividend, dataBE, 64);
    uint8_t remainder[64]{};
    if (cmp_buf(dividend, divisor.dataBE) < 0) {
      uint512_t r;
      std::memcpy(r.dataBE, dividend, 64);
      return r;
    }
    for (int i = 0; i < 512; ++i) {
      lshift1_buf(remainder);
      if (get_bit_from_buf(dividend, i)) remainder[63] |= 1;
      if (cmp_buf(remainder, divisor.dataBE) >= 0) sub_buf_inplace(remainder, divisor.dataBE);
    }
    uint512_t r;
    std::memcpy(r.dataBE, remainder, 64);
    return r;
  }

  uint512_t& operator+=(const uint512_t& b) {
    add_buf_inplace(dataBE, b.dataBE);
    return *this;
  }

  uint512_t& operator-=(const uint512_t& b) {
    sub_buf_inplace(dataBE, b.dataBE);
    return *this;
  }

  uint512_t& operator*=(const uint512_t& b) {
    uint8_t tmp[64]{};
    for (int i = 63; i >= 0; --i) {
      uint16_t carry = 0;
      for (int j = 63; j >= 0; --j) {
        int k = i + j - 63;
        if (k < 0) break;
        uint32_t prod = tmp[k] + static_cast<uint32_t>(dataBE[i]) * b.dataBE[j] + carry;
        tmp[k] = static_cast<uint8_t>(prod & 0xFF);
        carry = static_cast<uint16_t>(prod >> 8);
      }
    }
    std::memcpy(dataBE, tmp, 64);
    return *this;
  }

  uint512_t& operator/=(const uint512_t& b) {
    uint8_t dividend[64];
    std::memcpy(dividend, dataBE, 64);
    uint8_t quotient[64]{};
    uint8_t remainder[64]{};
    for (int i = 0; i < 512; ++i) {
      lshift1_buf(remainder);
      if (get_bit_from_buf(dividend, i)) remainder[63] |= 1;
      if (cmp_buf(remainder, b.dataBE) >= 0) {
        sub_buf_inplace(remainder, b.dataBE);
        set_bit_in_buf(quotient, i);
      }
    }
    std::memcpy(dataBE, quotient, 64);
    return *this;
  }

  uint512_t& operator%=(const uint512_t& b) {
    uint8_t dividend[64];
    std::memcpy(dividend, dataBE, 64);
    uint8_t remainder[64]{};
    for (int i = 0; i < 512; ++i) {
      lshift1_buf(remainder);
      if (get_bit_from_buf(dividend, i)) remainder[63] |= 1;
      if (cmp_buf(remainder, b.dataBE) >= 0) sub_buf_inplace(remainder, b.dataBE);
    }
    std::memcpy(dataBE, remainder, 64);
    return *this;
  }

  uint512_t& operator<<=(size_t shift) {
    if (shift >= 512) {
      clear_buf(dataBE);
      return *this;
    }
    uint8_t tmp[64]{};
    size_t byteShift = shift / 8;
    uint8_t bitShift = shift % 8;
    for (int i = 0; i < 64; ++i) {
      int dst = i - static_cast<int>(byteShift);
      if (dst < 0 || dst >= 64) continue;
      uint16_t val = static_cast<uint16_t>(dataBE[i]) << bitShift;
      tmp[dst] = static_cast<uint8_t>(val & 0xFF);
      if (dst - 1 >= 0 && bitShift) tmp[dst - 1] |= static_cast<uint8_t>(val >> 8);
    }
    std::memcpy(dataBE, tmp, 64);
    return *this;
  }

  uint512_t& operator>>=(size_t shift) {
    if (shift >= 512) {
      clear_buf(dataBE);
      return *this;
    }
    uint8_t tmp[64]{};
    size_t byteShift = shift / 8;
    uint8_t bitShift = shift % 8;
    for (int i = 63; i >= 0; --i) {
      int dst = i + static_cast<int>(byteShift);
      if (dst < 0 || dst >= 64) continue;
      uint16_t val = static_cast<uint16_t>(dataBE[i]) >> bitShift;
      tmp[dst] = static_cast<uint8_t>(val & 0xFF);
      if (dst + 1 < 64 && bitShift) tmp[dst + 1] |= static_cast<uint8_t>((dataBE[i] << (8 - bitShift)) & 0xFF);
    }
    std::memcpy(dataBE, tmp, 64);
    return *this;
  }

  operator std::string() const {
    uint8_t tmp[64];
    std::memcpy(tmp, dataBE, 64);
    bool all_zero = true;
    for (int i = 0; i < 64; ++i)
      if (tmp[i]) {
        all_zero = false;
        break;
      }
    if (all_zero) return "0";
    std::string out;
    while (true) {
      bool zero = true;
      for (int i = 0; i < 64; ++i)
        if (tmp[i]) {
          zero = false;
          break;
        }
      if (zero) break;
      uint16_t rem = 0;
      for (int i = 0; i < 64; ++i) {
        uint16_t cur = (rem << 8) | tmp[i];
        tmp[i] = static_cast<uint8_t>(cur / 10);
        rem = cur % 10;
      }
      out.insert(out.begin(), char('0' + rem));
    }
    return out;
  }

#define OV_CAST(size)                                                               \
  operator uint##size##_t *() { return reinterpret_cast<uint##size##_t*>(dataBE); } \
  operator const uint##size##_t *() { return reinterpret_cast<uint##size##_t*>(dataBE); }

  operator uint8_t*() { return dataBE; }
  operator const uint8_t*() const { return dataBE; }
  OV_CAST(16)
  OV_CAST(32)
  OV_CAST(64)
#undef OV_CAST

  std::string to_hex() const {
    static constexpr char hexmap[] = "0123456789abcdef";
    std::string out;
    out.reserve(128);
    for (int i = 0; i < 64; ++i) {
      uint8_t b = dataBE[i];
      out.push_back(hexmap[b >> 4]);
      out.push_back(hexmap[b & 0xF]);
    }
    return out;
  }

  bool operator<(const uint512_t& b) const { return std::memcmp(dataBE, b.dataBE, 64) < 0; }
  bool operator>(const uint512_t& b) const { return std::memcmp(dataBE, b.dataBE, 64) > 0; }
  bool operator<=(const uint512_t& b) const { return std::memcmp(dataBE, b.dataBE, 64) <= 0; }
  bool operator>=(const uint512_t& b) const { return std::memcmp(dataBE, b.dataBE, 64) >= 0; }
  bool operator==(const uint512_t& b) const { return std::memcmp(dataBE, b.dataBE, 64) == 0; }
  bool operator!=(const uint512_t& b) const { return std::memcmp(dataBE, b.dataBE, 64) != 0; }

  uint8_t& operator[](size_t index) { return dataBE[index]; }
};

inline std::string operator+(const std::string& s, const uint512_t& v) { return s + static_cast<std::string>(v); }
inline std::string& operator+=(std::string& s, const uint512_t& v) {
  s += static_cast<std::string>(v);
  return s;
}

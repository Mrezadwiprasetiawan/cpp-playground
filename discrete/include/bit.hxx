#pragma once
#include <concepts>
#include <cstdint>

inline uint64_t bswap64(uint64_t x) {
  return (x << 56) | ((x << 40) & 0x00FF000000000000ULL) | ((x << 24) & 0x0000FF0000000000ULL) | ((x << 8) & 0x000000FF00000000ULL) |
         ((x >> 8) & 0x00000000FF000000ULL) | ((x >> 24) & 0x0000000000FF0000ULL) | ((x >> 40) & 0x000000000000FF00ULL) | (x >> 56);
}
inline uint32_t bswap32(uint32_t x) { return (x << 24) | ((x << 8) & 0x00FF0000) | ((x >> 8) & 0x0000FF00) | (x >> 24); }
inline uint16_t bswap16(uint16_t x) { return (x << 8) | (x >> 8); }
template <std::integral T>
inline T byteswap(T x) {
  if constexpr (sizeof(T) == 8) return bswap64(static_cast<uint64_t>(x));
  else if constexpr (sizeof(T) == 4) return bswap32(static_cast<uint32_t>(x));
  else if constexpr (sizeof(T) == 2) return bswap16(static_cast<uint16_t>(x));
  else return x;
}
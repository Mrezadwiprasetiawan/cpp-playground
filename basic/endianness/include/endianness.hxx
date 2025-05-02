#include <cstdint>

enum Endianness { BIG, LITTLE };

inline bool is_little_endian() {
  uint16_t testdata = 0x0100;
  uint8_t *testarr = reinterpret_cast<uint8_t *>(&testdata);
  if (testarr[0] & 1) return false;
  return true;
}

inline bool is_big_endian() { return !is_little_endian(); }

const Endianness CURRENT_ENDIANNESS =
    is_little_endian() ? Endianness::LITTLE : Endianness::BIG;

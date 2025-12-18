#pragma once

#include <cstdint>

inline static bool is_big_endian() {
  uint16_t data = 0xff00;
  return *reinterpret_cast<uint8_t*>(&data);
}
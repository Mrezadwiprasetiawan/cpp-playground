#pragma once

#include <array>
#include <bit>
#include <cstdint>
#include <string>

#include "sha3-512.hxx"
#include "uint512_t.hxx"

struct Header {
 private:
  static constexpr std::array<char, 4> magicBE_raw = {0x0d, 0x0a, 0x12, 0x0e};

 public:
  static constexpr std::array<char, 4> magic = []() constexpr {
    if constexpr (std::endian::native == std::endian::little) return std::array<char, 4>{magicBE_raw[3], magicBE_raw[2], magicBE_raw[1], magicBE_raw[0]};
    else return magicBE_raw;
  }();

  static inline const std::string version_STR = "000001";
  static inline const uint64_t version = 1;
};

class Block {
  Block *prev = nullptr;
  uint512_t hash{};
  std::string hashHex{};
  uint512_t nonce{};
  bool verified = false;

 public:
  explicit Block() = default;

  void generate_hash(const Block *prevBlock, const uint512_t &nonceVal) {
    using namespace std;
    string input;
    if (prevBlock) input += prevBlock->hashHex;
    input.append(Header::magic.begin(), Header::magic.end());
    input += Header::version_STR;
    input += to_string(Header::version);
    input += static_cast<std::string>(nonceVal);
    hash = SHA3_512::hash(input);
    hashHex = hash.to_hex();
  }

  static bool check(double difficulty, Block chBlock) {
    uint512_t test = chBlock.hash;
    chBlock.generate_hash(chBlock.prev, chBlock.nonce);
    if (chBlock.hash == test) {
      chBlock.verified = true;
      return true;
    }
    chBlock.hash = test;
    return false;
  }
};

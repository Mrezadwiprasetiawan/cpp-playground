#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "blockchain.hxx"

class Wallet {
  double balance;
  std::array<uint64_t, 8> identifier_BE;
  std::array<uint64_t, 4> privKey_BE;
  std::array<uint64_t, 4> pubKey_BE;
  std::string blockChainFilename = "blockswb.bin";
  std::vector<Block> blockChainCache;

 public:
  explicit Wallet(const std::array<uint64_t, 8> &identifier_BE, const std::array<uint64_t, 4> &privKey_BE, const std::array<uint64_t, 4> &pubKey_BE)
      : identifier_BE(identifier_BE), privKey_BE(privKey_BE), pubKey_BE(pubKey_BE) {}
  double get_balance() { return balance; }
  bool add_balance(double balance, Block rewardBlock) {}
};
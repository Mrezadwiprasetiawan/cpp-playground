#pragma once
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <vector>

#include "sha3-512.hxx"
#include "uint512_t.hxx"

struct Target512 {
  uint512_t value{};

  static Target512 initial() {
    Target512 t;
    t.value[0] = 0x00;
    t.value[1] = 0x00;
    t.value[2] = 0x00;
    t.value[3] = 0xFF;
    return t;
  }

  bool check(const uint512_t& i) const { return i <= value; }

  void update_difficulty(double factor) {
    using namespace std;

    constexpr uint64_t SCALE = 10000;
    uint512_t scale(SCALE);

    if (factor < 1.0) {
      double inv = 1.0 / factor;
      uint64_t scaled = static_cast<uint64_t>(inv * SCALE);
      value /= scale;
      value *= uint512_t(scaled);
    } else {
      uint64_t scaled = static_cast<uint64_t>(factor * SCALE);
      value /= uint512_t(scaled);
      value *= scale;
    }
  }
};

struct Miner512 {
  Target512 target;
  std::mutex print_mtx;
  std::mutex nonce_mtx;
  uint512_t winning_nonce{};
  int num_threads = 4;
  uint64_t sumTime = 0;

  Miner512() : target(Target512::initial()), winning_nonce(0) {}

  void mine_thread(const std::string& weight, int thread_id, bool& block_found, std::array<std::string, 64>& thread_hashes) {
    using namespace std;
    uint512_t local_nonce = uint512_t(static_cast<uint64_t>(thread_id));
    while (true) {
      {
        lock_guard<mutex> lg(nonce_mtx);
        if (block_found) break;
      }

      stringstream ss;
      ss << local_nonce.to_hex() << weight;
      uint512_t hash = SHA3_512::hash(ss.str());
      thread_hashes[thread_id] = hash.to_hex();

      if (target.check(hash)) {
        lock_guard<mutex> lg(nonce_mtx);
        if (winning_nonce == uint512_t(0) || local_nonce < winning_nonce) winning_nonce = local_nonce;
        block_found = true;
        break;
      }

      local_nonce += uint512_t(num_threads);
    }
  }

  void mine_concurrent(int threads = 4) {
    using namespace std;
    num_threads = threads;
    uint64_t total_found = 0;
    string prev_hash = "";

    while (true) {
      bool block_found = false;
      winning_nonce = uint512_t(0);
      array<string, 64> thread_hashes;
      vector<thread> thread_pool;

      {
        lock_guard<mutex> lg(print_mtx);
        cout << "Current target is " << target.value.to_hex() << endl << endl;
      }
      auto start = chrono::high_resolution_clock::now();

      for (int i = 0; i < num_threads; ++i) thread_pool.emplace_back(&Miner512::mine_thread, this, ref(prev_hash), i, ref(block_found), ref(thread_hashes));

      for (auto& t : thread_pool) t.join();

      auto end = chrono::high_resolution_clock::now();
      auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
      if (duration == 0) duration = 1;
      sumTime += duration;
      {
        lock_guard<mutex> lg(print_mtx);
        cout << "Block found! Winning nonce = " << winning_nonce.to_hex() << " Time = " << duration << "ms Total blocks found = " << (++total_found) << endl;
        cout << "Hashes from threads:" << endl;
        for (int i = 0; i < num_threads; ++i) cout << "[Thread " << i << "] hash=" << thread_hashes[i] << endl;
        cout << endl;
      }

      prev_hash = thread_hashes[0];
      for (int i = 1; i < num_threads; ++i)
        if (!thread_hashes[i].empty() && (winning_nonce % uint512_t(num_threads) == uint512_t(i))) prev_hash = thread_hashes[i];

      if (!(total_found % 10)) {
        double factor = 60000.0 / static_cast<double>(sumTime) * 10;  // target 60s per block }
        target.update_difficulty(factor);
        sumTime = 0;
        {
          lock_guard<mutex> lg(print_mtx);
          cout << "Difficulty updated with factor = " << factor << endl << endl;
        }
      }
    }
  }
};

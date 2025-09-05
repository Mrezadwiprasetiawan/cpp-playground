#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <vector>

template <std::integral I>
class collatz_matrix {
  std::vector<std::vector<std::vector<I>>> caches;

 public:
  explicit collatz_matrix() {}
  std::vector<std::vector<I>> operator[](size_t size) {
    using namespace std;
    if (caches.size() > size) {
      if (caches[0].size() < size) {
        vector<vector<uint64_t>> matrix;
        for (size_t i = 0; i < size; ++i) {
          vector<uint64_t> row;
          for (size_t j = 0; j < size; ++j) row.push_back(j);
        }
      }
      return caches[size];
    }
  }
  void clear_cache() { caches.clear(); }
};
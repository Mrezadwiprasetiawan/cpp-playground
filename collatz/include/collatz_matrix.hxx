#pragma once

#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// helper struct
template <typename T>
requires(std::integral<T> || std::floating_point<T>) struct Vec4 {
  T w, x, y, z;
};

class CollatzMatrix {
  std::vector<std::vector<std::vector<uint64_t>>> matrices;

 private:
  void init(size_t longestPath, size_t oddStarter) {
    using namespace std;
    vector<vector<uint64_t>> matrix;
    for (size_t row = 0; row < longestPath; ++row) {
      vector<uint64_t> rowVector;
      for (size_t col = 0; col < longestPath; ++col) rowVector.push_back(pow(3, col) * pow(2, row) * oddStarter);
      matrix.push_back(std::move(rowVector));
    }
    matrices[oddStarter / 3] = matrix;
  }

 public:
  explicit CollatzMatrix() = delete;
  explicit CollatzMatrix(uint64_t highestOddStarter, uint64_t longestPath) {
    size_t matricesSize = highestOddStarter / 6 + 1 << 1;
    matrices.resize(matricesSize);
    if (longestPath) {
      for (size_t i = 0; i < matricesSize >> 1; ++i) {
        size_t mul6i = 6 * i;
        init(longestPath, mul6i + 1);
        init(longestPath, mul6i + 5);
      }
    }
  }

  /**
  @return vector yang berisi jalur dalam bentuk nilai positif di sumbu k sebelum mencapai nilai positif k = 1
  @param n nilai awal yang akan di lacak jalurnya
  @note nilai n berada pada sumbu k namun hanya nilai positif yang ditrack
  **/
  std::vector<Vec4<uint64_t>> find_path(uint64_t n) {
    std::vector<Vec4<uint64_t>> res;
    if (!n) return res;
    while (!(n & 1)) n >>= 1;
    n = n + 1 >> 1;
    while (n != 1) {
      size_t row = 0, col = 0;
      while (!(n & 1)) {
        n >>= 1;
        ++row;
      }
      while (!(n % 3)) {
        n /= 3;
        ++col;
      }
      size_t matIndex = n / 3;
      if (matIndex >= matrices.size())
        throw std::out_of_range("matrices size out of range. size = " + std::to_string(matrices.size()) + "while the index = " + std::to_string(matIndex));
      while (row) {
        res.push_back({matIndex, row, col, matrices[matIndex][row][col]});
        ++col;
        --row;
      }
      res.push_back({matIndex, row, col, matrices[matIndex][row][col]});
      n = res[res.size() - 1].z * 3 - 1 >> 1;
      while (!(n & 1)) n >>= 1;

      // konversi kembali ke sumbu k
      n = n + 1 >> 1;
    }
    res.push_back({0, 0, 0, 1});
    return res;
  }

  void printMatrices() {
    using namespace std;
    for (int i = 0; i < matrices.size(); ++i) {
      cout << "debug matrix ke " << i << endl;
      for (int row = 0; row < matrices[i].size(); ++row) {
        for (int col = 0; col < matrices[i][row].size(); ++col) cout << matrices[i][row][col] << " ";
        cout << endl;
      }
      cout << endl;
    }
  }
};
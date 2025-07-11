/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// temporary not used
// #include "big_int.hxx"

namespace Discrete {
class Fibonacci {
 private:
  std::vector<std::vector<uint64_t>> values;
  size_t                             lastLimit = 0;
  std::vector<std::string>           values_str;
  const std::string                  TWO_POW_64 = "18446744073709551616";

  std::string multiplyStrings(const std::string &a, const std::string &b) {
    if (a == "0" || b == "0") return "0";
    int              m = a.size(), n = b.size();
    std::vector<int> res(m + n, 0);
    for (int i = m - 1; i >= 0; i--) {
      for (int j = n - 1; j >= 0; j--) {
        int mul = (a[i] - '0') * (b[j] - '0');
        int p1 = i + j, p2 = i + j + 1;
        int sum  = mul + res[p2];
        res[p2]  = sum % 10;
        res[p1] += sum / 10;
      }
    }

    std::string result;
    for (int num : res)
      if (!(result.empty() && num == 0)) result.push_back(num + '0');
    return result.empty() ? "0" : result;
  }

  std::string multiplyBy2To64(const std::string &numStr) { return multiplyStrings(numStr, TWO_POW_64); }

  std::string addUint64ToString(const std::string &numStr, uint64_t add) {
    std::string result;
    int         carry   = 0;
    int         i       = numStr.size() - 1;
    uint64_t    add_val = add;
    while (i >= 0 || add_val > 0 || carry > 0) {
      int sum = carry;
      if (i >= 0) sum += numStr[i--] - '0';
      if (add_val > 0) {
        sum     += add_val % 10;
        add_val /= 10;
      }
      carry = sum / 10;
      result.push_back(sum % 10 + '0');
    }

    std::reverse(result.begin(), result.end());
    size_t start = result.find_first_not_of('0');
    return (start != std::string::npos) ? result.substr(start) : "0";
  }

  std::vector<uint64_t> add64_ext(const std::vector<uint64_t> &a, const std::vector<uint64_t> &b) {
    std::vector<uint64_t> res;
    size_t                max_size = std::max(a.size(), b.size());
    std::vector<uint64_t> a_copy   = a;
    std::vector<uint64_t> b_copy   = b;
    a_copy.resize(max_size, 0);
    b_copy.resize(max_size, 0);
    uint64_t carry = 0;
    for (size_t i = 0; i < max_size; ++i) {
      uint64_t sum_limb  = a_copy[i] + b_copy[i];
      uint64_t carry1    = sum_limb < a_copy[i] ? 1 : 0;
      sum_limb          += carry;
      uint64_t carry2    = sum_limb < carry ? 1 : 0;
      res.push_back(sum_limb);
      carry = carry1 + carry2;
    }
    if (carry) res.push_back(carry);
    while (!res.empty() && res.back() == 0) res.pop_back();
    return res.empty() ? std::vector<uint64_t>{0} : res;
  }

  std::vector<std::string> decode() {
    std::vector<std::string> res;
    for (const auto &fbnc : values) {
      std::string current = "0";
      for (auto it = fbnc.rbegin(); it != fbnc.rend(); ++it) {
        current = multiplyBy2To64(current);
        current = addUint64ToString(current, *it);
      }
      res.push_back(current);
    }
    return res;
  }

  void generate(size_t limit) {
    if (limit < lastLimit) {
      values.resize(limit);
      return;
    }
    if (values.empty()) {
      values.push_back({0});  // F0
      values.push_back({1});  // F1
    }
    size_t currentSize = values.size();
    if (limit <= currentSize) {
      lastLimit  = limit;
      values_str = decode();
      return;
    }
    values.resize(limit);
    for (size_t i = currentSize; i < limit; ++i) {
      if (i < 2) continue;
      values[i] = add64_ext(values[i - 1], values[i - 2]);
    }
    lastLimit  = limit;
    values_str = decode();
  }

 public:
  std::vector<std::string> get_all(size_t limit) {
    if (limit == lastLimit) return values_str;
    generate(limit);
    return values_str;
  }

  std::string get_index(size_t index) { return get_all(index + 1)[index]; }
};
}  // namespace Discrete
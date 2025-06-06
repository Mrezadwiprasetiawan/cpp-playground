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
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// most 64bit on the highest index
class Big_int {
 private:
  std::vector<uint64_t> values;
  // mutable = non logic state
  mutable std::string values_str;
  bool negative = false;
  const static uint16_t max_thread;

  // For positive only
  Big_int(std::vector<uint64_t> values) : values(values) {}
  std::string uint64_to_string(uint64_t val) {
    std::string res;
    if (!val) return "0";
    while (val) {
      res.insert(res.begin(), val % 10 + '0');
      val /= 10;
    }
    return res;
  }

  uint64_t string_to_uint64_t(const std::string &val) {
    uint64_t res = 0;
    for (auto c : val) res = res * 10 + c - '0';
    return res;
  }

  void mul_2_64_add_other(std::string &val, uint64_t other) {
    using namespace std;
    if (val.empty() || val == "0") {
      val = uint64_to_string(other);
      return;
    }
    std::string res;
    // multiply
    std::vector<int> tmpres(val.size() + Big_int::two_pow_64.size(), 0);
    for (size_t i = 0; i < val.size(); ++i) {
      size_t rev_i = val.size() - 1 - i;
      for (size_t j = 0; j < Big_int::two_pow_64.size(); ++j) {
        size_t rev_j = Big_int::two_pow_64.size() - 1 - j;
        int mul = (val[rev_i] - '0') * (Big_int::two_pow_64[rev_j] - '0');
        size_t p1 = rev_i + rev_j, p2 = p1 + 1;
        int sum = tmpres[p2] + mul;
        tmpres[p2] = sum % 10;
        tmpres[p1] += sum / 10;
      }
    }

    if (!other) return;

    // add the other
    std::string b = uint64_to_string(other);
    int carry = 0;

    for (size_t i = 0; i < b.size(); ++i) {
      size_t rev_i = tmpres.size() - 1 - i;
      size_t rev_i_b = b.size() - 1 - i;
      int sum = b[rev_i_b] - '0' + tmpres[rev_i];
      sum += carry;
      carry = sum / 10;
      tmpres[rev_i] = sum % 10;
    }

    for (int i = 0; carry && i < tmpres.size(); ++i) {
      int sum = tmpres[tmpres.size() - i - 1] + carry;
      tmpres[tmpres.size() - i - 1] = sum % 10;
      carry = sum / 10;
    }

    if (carry) tmpres.insert(tmpres.begin(), 1, carry);

    while (!tmpres[0]) tmpres.erase(tmpres.begin());
    res.resize(tmpres.size());
    for (size_t i = 0; i < tmpres.size(); ++i) res[i] = tmpres[i] + '0';

    val = res;
  }

  // TODO : Implement this method
  void div_mod_2_64(std::string &val, std::string *remainder) const {
    if (val.size() < Big_int::two_pow_64.size()) {
      if (remainder) *remainder = val;
      val = "";
      return;
    }
    std::vector<int> sub_res(val.size(), 0);
    size_t offset = val.size() - Big_int::two_pow_64.size();
    uint64_t div_res = 0;
    bool greater_equal = true;
    bool carry = 0;
    while (greater_equal) {
      for (size_t i = 0; i < val.size(); ++i) {
        size_t rev_val_i = val.size() - 1 - i;
        size_t rev_2_64_i = rev_val_i - offset;
      }
    }
  }

 public:
  // done
  static const std::string two_pow_64;
  explicit Big_int(std::vector<uint64_t> values, bool negative) noexcept : values(values), negative(negative) {}

  Big_int(uint64_t value) {
    if (value < (1ULL << 63)) values = {value};
    else {
      values = {~value + 1};
      this->negative = true;
    }
  }

  Big_int(std::string value) {
    std::string rem = "0";
    std::vector<uint64_t> chunks;
    while (!value.empty()) {
      div_mod_2_64(value, &rem);
      chunks.push_back(string_to_uint64_t(rem));
    }
  }

  std::string to_string() const {}

#define FUNC_OP(op) Big_int op(const Big_int &other) const

  FUNC_OP(add) {
    std::vector<uint64_t> this_copy = this->values, other_copy = other.values, result;

    // for flag and result
    bool negative = false;

    // padding
    if (this->values.size() != other.values.size()) {
      size_t pad = this->values.size() > other.values.size() ? this->values.size() - other.values.size() : other.values.size() - this->values.size();
      this->values.size() > other.values.size() ? other_copy.insert(other_copy.begin(), pad, 0) : this_copy.insert(this_copy.begin(), pad, 0);
    }

    // inverse bit and add 1 for negative
    if (this->negative != other.negative) {
      negative = true;
      bool carry = 1;
      if (this->negative) {
        for (size_t i = 0; i < this_copy.size(); ++i) {
          this_copy[i] = ~this_copy[i] + carry;
          carry = this_copy[i] == 0;
        }
      } else {
        for (size_t i = 0; i < this_copy.size(); ++i) {
          other_copy[i] = ~other_copy[i] + carry;
          carry = other_copy[i] == 0;
        }
      }
    }

    bool carry = false;
    for (size_t i = 0; i < this_copy.size(); ++i) {
      result.push_back(this_copy[i] + other_copy[i] + carry);
      carry = (result[i] < this_copy[i] || (carry && result[i] == this_copy[i]));
    }

    // if carry (outside to make sure if false skip) but both not equal
    // negative=false, if not add the carry
    if (carry) {
      if (negative) negative = false;
      else {
        result.push_back(1);
        if (this->negative) negative = true;
      }
    }

    return Big_int(result, negative);
  }

  FUNC_OP(min) {
    Big_int big_new(other.values, !other.negative);
    return this->add(big_new);
  }
  FUNC_OP(mul) {
    size_t res_bit = (this->values.size() << 6) + (other.values.size() << 6);
    if (res_bit < this->values.size()) throw std::runtime_error("overflow reached");

    Big_int res(0);

    if (this->negative ^ other.negative) res.negative = true;

    for (size_t i = 0; i < other.values.size(); ++i) {
      uint64_t factor = other.values[i];
      for (uint8_t j = 1; j < uint8_t(1 << 6); ++j) {
        if (factor & (1ULL << j)) res = res + (*this << j);
        factor = factor >> 1;
      }
    }
    return res;
  }

  FUNC_OP(div) {}
#undef FUNC_OP

#define OPERATOR_DECL(op, alter) \
  Big_int operator op(const Big_int &other) const { return alter(other); }
  OPERATOR_DECL(+, add);
  OPERATOR_DECL(-, min);
  OPERATOR_DECL(*, mul);
  OPERATOR_DECL(/, div);
#undef OPERATOR_DECL
  // special case
  Big_int operator++(int) { return add(Big_int(1)); }
  Big_int operator--(int) { return min(Big_int(1)); }
  bool operator!() { return (values.size() == 1 && values[0] == 0) || values.empty(); }

#define BITWISE(op) Big_int operator op(const Big_int &other) const

  BITWISE(&) {}
#undef BITWISE

#define OPERATOR_LOGIC_DECL(op, alter)    \
  bool alter(const Big_int &other) const; \
  bool operator op(const Big_int &other) const { return alter(other); }

  OPERATOR_LOGIC_DECL(>, gt);
  OPERATOR_LOGIC_DECL(>=, gteq);
  OPERATOR_LOGIC_DECL(<, lt);
  OPERATOR_LOGIC_DECL(<=, lteq);
  OPERATOR_LOGIC_DECL(==, equal);
  OPERATOR_LOGIC_DECL(!=, noteq);
  OPERATOR_LOGIC_DECL(&&, andand);
  OPERATOR_LOGIC_DECL(||, oror);

  Big_int shift_left(uint64_t k) const {
    std::vector<uint64_t> res_values(this->values);
    if (!k) return Big_int(res_values, this->negative);
    assert((k >> 6) + res_values.size() > res_values.size());
    if (k >> 6) res_values.insert(res_values.end(), k >> 6, 0);
    uint64_t carry = 0;
    uint64_t bits = (k & 63ULL);
    // mencegah shifting 64 kali jika k kelipatan 64 karena 64-0 dilewatkan
    if (bits) {
      for (auto it = res_values.rbegin(); it != res_values.rend(); ++it) {
        uint64_t tmp = *it << bits | carry;
        carry = *it >> (64ULL - bits);
        *it = tmp;
      }
    }
    return Big_int(res_values, this->negative);
  }

  Big_int operator<<(uint64_t k) const { return shift_left(k); }

  Big_int shift_right(uint64_t k) const {
    std::vector<uint64_t> res_values(this->values);
    if (!k) return Big_int(res_values, this->negative);
    if (k >> 6 >= res_values.size()) return Big_int({0}, this->negative);
    if (k >> 6) res_values.resize(res_values.size() - (k >> 6));
    uint64_t carry = 0;
    uint64_t bits = (k & 63ULL);
    if (bits) {
      for (auto it = res_values.begin(); it != res_values.end(); ++it) {
        uint64_t tmp = *it >> bits | carry;
        carry = *it << (64ULL - bits);
        *it = tmp;
      }
    }
    while (res_values.size() > 1 && !res_values.back()) res_values.pop_back();
    return Big_int(res_values, this->negative);
  }

  Big_int operator>>(uint64_t k) const { return shift_right(k); }
};  // END Big_int class
// static member init
const std::string two_pow_64 = "18446744073709551616";

inline Big_int operator""_big(unsigned long long i) { return Big_int(i); }
inline Big_int operator""_big(const char *str, size_t size) { return Big_int(std::string(str)); };

inline std::ostream &operator<<(const std::ostream &a, const Big_int &other) { return a << other.to_string(); }

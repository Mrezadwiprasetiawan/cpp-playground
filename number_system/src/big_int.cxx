#include "big_int.hxx"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string>

std::string uint64_to_string(uint64_t val) {
  std::string res;
  if (!val) return "0";
  while (val) {
    res.insert(res.begin(), val % 10 + '0');
    val /= 10;
  }
  return res;
}

const std::string big_int::two_pow_64 = "18446744073709551616";

void mul_2_64_add_other(std::string& val, uint64_t other) {
  using namespace std;
  if (val.empty() || val == "0") {
    val = uint64_to_string(other);
    return;
  }
  std::string res;
  // multiply
  std::vector<int> tmpres(val.size() + big_int::two_pow_64.size(), 0);
  for (size_t i = 0; i < val.size(); ++i) {
    size_t rev_i = val.size() - 1 - i;
    for (size_t j = 0; j < big_int::two_pow_64.size(); ++j) {
      size_t rev_j = big_int::two_pow_64.size() - 1 - j;
      int mul = (val[rev_i] - '0') * (big_int::two_pow_64[rev_j] - '0');
      size_t p1 = rev_i + rev_j, p2 = p1 + 1;
      int sum = tmpres[p2] + mul;
      tmpres[p2] = sum % 10;
      tmpres[p1] += sum / 10;
    }
  }

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

void div_mod_2_64(std::string& val, std::string remainder) {}

std::string big_int::to_string() const {
  std::string s;
  uint64_t rem = 0;
  for (auto u64 = this->values.rbegin(); u64 != this->values.rend(); ++u64)
    mul_2_64_add_other(s, *u64);

  if (this->negative) s.insert(s.begin(), '-');

  return s;
}

big_int big_int::shift_left(size_t k) const {
  std::vector<uint64_t> res_values(this->values);
  if (!k) return big_int(res_values, this->negative);
  assert((k >> 6) + res_values.size() > res_values.size());
  if (k >> 6) res_values.insert(res_values.end(), k >> 6, 0);
  uint64_t carry = 0;
  uint64_t bits = (k & 63ULL);
  if (bits) {
    for (auto it = res_values.rbegin(); it != res_values.rend(); ++it) {
      uint64_t tmp = *it << bits | carry;
      carry = *it >> (64ULL - bits);
      *it = tmp;
    }
  }
  return big_int(res_values, this->negative);
}

big_int big_int::shift_right(size_t k) const {
  std::vector<uint64_t> res_values(this->values);
  if (!k) return big_int(res_values, this->negative);
  if (k >> 6 >= res_values.size()) return big_int({0}, this->negative);
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
  return big_int(res_values, this->negative);
}

big_int big_int::add(const big_int& other) const {
  std::vector<uint64_t> this_copy = this->values, other_copy = other.values,
                        result;

  // for flag and result
  bool negative = false;

  // padding
  if (this->values.size() != other.values.size()) {
    size_t pad = this->values.size() > other.values.size()
                     ? this->values.size() - other.values.size()
                     : other.values.size() - this->values.size();
    this->values.size() > other.values.size()
        ? other_copy.insert(other_copy.begin(), pad, 0)
        : this_copy.insert(this_copy.begin(), pad, 0);
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
    if (negative)
      negative = false;
    else {
      result.push_back(1);
      if (this->negative) negative = true;
    }
  }

  return big_int(result, negative);
}

big_int big_int::min(const big_int& other) const {
  big_int big_new(other.values, !other.negative);
  return this->add(big_new);
}

big_int big_int::mul(const big_int& other) const {
  size_t res_bit = (this->values.size() << 6) + (other.values.size() << 6);
  if (res_bit < this->values.size())
    throw std::runtime_error("overflow reached");

  big_int res(0);

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

#include "big_int.hxx"

#include <cstdint>
#include <cstring>
#include <sstream>
#include <stdexcept>

std::string uint64_to_string(uint64_t val) {
  std::string res;
  while (val) {
    res.insert(res.begin(), val % 10 + '0');
    val /= 10;
  }
  return res;
}

const std::string big_int::two_pow_64 = "18446744073709551616";
std::string mul_2_64_add_other(std::string val, uint64_t other) {
  if (val.empty() || val == "0") {
    return "0";
  }

  std::string res(val.size() + big_int::two_pow_64.size(), '0');
  // multiply
  for (size_t i = 0; i < val.size(); ++i) {
    size_t rev_i = val.size() - 1 - i;
    for (size_t j = 0; j < big_int::two_pow_64.size(); ++j) {
      size_t rev_j = big_int::two_pow_64.size() - 1 - j;
      int mul = (val[rev_i] - '0') * (big_int::two_pow_64[rev_j] - '0');
      char p1 = mul % 10 + '0';
      char p2 = mul / 10 + '0';
      res[rev_i + rev_j] += p1;
      res[rev_i + rev_j + 1] += p2;
    }
  }

  // add the other
  std::string b = uint64_to_string(other);

  for (size_t i = 0; i < b.size(); ++i) {
    size_t rev_i = b.size() - 1 - i;
    for (size_t j = 0; j < res.size(); ++j) {
      size_t rev_j = res.size() - 1 - j;
      int add = (b[rev_i] - '0') + (res[rev_j] - '0');
      char p1 = add % 10 + '0';
      char p2 = add / 10 + '0';
      res[rev_j] = p1;
      if (p2 != '0') res[rev_j - 1] += p2;
    }
  }

  return res;
}

std::string big_int::to_string() const {
  std::stringstream ss;
  if (this->negative) ss << "-";
  uint64_t rem = 0;
  for (auto u64 = this->values.rbegin(); u64 != this->values.rend(); ++u64) {
  }
  return ss.str();
}

big_int big_int::shift_left(uint64_t k) const {
  std::vector<uint64_t> res_values(this->values);
  if (!k) return big_int(res_values, this->negative);

  // resize k/64+1 karena elementnya uint64_t
  res_values.resize((k >> 6) + 1, 0);
  uint64_t carry = 0;
  for (size_t i = 0; i < res_values.size(); ++i) {
    res_values[i] = res_values[i] + carry;
    if (res_values[i] < carry)
      carry = 1;
    else
      carry = 0;
    carry += res_values[i] >> (64 - k) & ((1 << (63 - k)) - 1);
    res_values[i] = res_values[i] << k;
  }
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
      if (factor & 1) res = res + (*this << j);
      factor = factor >> 1;
    }
  }
  return res;
}

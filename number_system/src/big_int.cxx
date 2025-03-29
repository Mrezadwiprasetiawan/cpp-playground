#include "big_int.hxx"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::string big_int::to_string() const {
  std::stringstream ss;
  if (this->negative) ss << "negative";
  for (size_t i = this->values.size() - 1; i > 0; ++i) {
    ss << i << " ";
  }
  if (this->values.size() == 1) ss << " ";
  ss << this->values[0];
  return ss.str();
}

big_int big_int::shift_left(uint64_t k) {
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

big_int big_int::add(const big_int& other) {
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

  // if carry outside(to make sure if false skip) but both not equal
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

big_int big_int::min(const big_int& other) {
  big_int big_new(other.values, !other.negative);
  return this->add(big_new);
}

big_int big_int::mul(const big_int& other) {
  size_t res_bit = (this->values.size() << 6) + (other.values.size() << 6);
  if (res_bit < this->values.size())
    throw std::runtime_error("overflow reached");

  big_int res(0);

  if (this->negative ^ other.negative) res.negative = true;

  for (size_t i = 0; i < other.values.size(); ++i) {
    uint64_t factor = other.values[i];
    for (uint8_t j = 1; j < uint8_t(1 << 6); ++j) {
      if (factor & 1) res = res + (*(this) << j);
      factor = factor << 1;
    }
  }
  return res;
}

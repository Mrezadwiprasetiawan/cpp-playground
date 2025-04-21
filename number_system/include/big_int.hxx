#ifndef BIG_INT_HXX
#define BIG_INT_HXX
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#define __BIGINT_OPERATOR_DECL(op, alter)    \
  big_int alter(const big_int &other) const; \
  big_int operator op(const big_int &other) const { return alter(other); }
#define __BIGINT_OPERATOR_LOGIC_DECL(op, alter) \
  bool alter(const big_int &other) const;       \
  bool operator op(const big_int &other) const { return alter(other); }

#define __BIGINT_OPERATOR_SHIFT_DECL(type)                           \
  big_int shift_left(type other) const;                              \
  big_int operator<<(type other) const { return shift_left(other); } \
  big_int shift_right(type other) const;                             \
  big_int operator>>(type other) const { return shift_right(other); }

// most 64bit on the highest index
class big_int {
 private:
  std::vector<uint64_t> values;
  std::string values_str;
  bool negative = false;

  // For positive only
  big_int(std::vector<uint64_t> values) : values(values) {}

  void set_negative(bool negative) { this->negative = negative; }

 public:
  static const std::string two_pow_64;
  big_int(std::vector<uint64_t> values, bool negative)
      : values(values), negative(negative) {}

  big_int(uint64_t value) {
    if (value < (1ULL << 63))
      values = {value};
    else {
      values = {~value + 1};
      this->negative = true;
    }
  }

  big_int(std::string value);

  // c/cpp aneh banget, perlu const di belakang untuk menandakan fungsi tidak
  // merubah big_int
  std::string to_string() const;

  static const big_int ONE;

  __BIGINT_OPERATOR_DECL(+, add);
  __BIGINT_OPERATOR_DECL(-, min);
  __BIGINT_OPERATOR_DECL(*, mul);
  __BIGINT_OPERATOR_DECL(/, div);

  // special case
  big_int operator++(int) { return add(ONE); }
  big_int operator--(int) { return min(ONE); }
  bool operator!() { return values_str.empty() || values_str == "0"; }

  __BIGINT_OPERATOR_DECL(&, _and)
  __BIGINT_OPERATOR_DECL(|, _or)
  __BIGINT_OPERATOR_DECL(^, _xor)

  __BIGINT_OPERATOR_LOGIC_DECL(>, gt);
  __BIGINT_OPERATOR_LOGIC_DECL(>=, gteq);
  __BIGINT_OPERATOR_LOGIC_DECL(<, lt);
  __BIGINT_OPERATOR_LOGIC_DECL(<=, lteq);
  __BIGINT_OPERATOR_LOGIC_DECL(==, equal);
  __BIGINT_OPERATOR_LOGIC_DECL(!=, noteq);
  __BIGINT_OPERATOR_LOGIC_DECL(&&, andand);
  __BIGINT_OPERATOR_LOGIC_DECL(||, oror);

  // uint64_t udah cukup, terlanjur bikin makronya males ngapus
  __BIGINT_OPERATOR_SHIFT_DECL(uint64_t);
#undef __BIGINT_OPERATOR_DECL
#undef __BIGINT_OPERATOR_LOGIC_DECL
#undef __BIGINT_OPERATOR_SHIFT_DECL
};

inline big_int operator""_big(unsigned long long i) { return big_int(i); }
inline big_int operator""_big(const char *str, size_t size) {
  return big_int(std::string(str));
};

inline std::ostream &operator<<(const std::ostream &a, const big_int &other) {
  return a << other.to_string();
}

#endif  // BIG_INT_HXX

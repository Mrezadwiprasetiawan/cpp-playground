#include "big_int.hxx"

Big_Integer Big_Integer::add(const Big_Integer &other) {
  size_t min =
      values.size() < other.values.size() ? values.size() : other.values.size();
  std::vector<uint64_t> res;
  uint64_t carry = 0;
  for (size_t i = min - 1; i >= 0; --i) {
    uint64_t sum = values[i] + other.values[i];
    uint64_t carry_tmp = sum < values[i] ? 1 : 0;
    if (carry) sum += carry;
    res.push_back(sum);
    carry = carry_tmp;
  }
  if (values.size() != other.values.size()) {
    std::vector<uint64_t> max =
        values.size() > other.values.size() ? *(&values) : *(&other.values);
    for (size_t i = min; i < max.size(); ++i) {
      uint64_t sum = values[i] + other.values[i];
      uint64_t carry_tmp = sum < values[i] ? 1 : 0;
      if (carry) sum += carry;
      res.push_back(sum);
      carry = carry_tmp;
    }
  }
  return Big_Integer(res);
}

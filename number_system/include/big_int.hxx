#ifndef BIG_INT_HXX
#define BIG_INT_HXX
#include <cstdint>
#include <string>
#include <vector>

#define __BIGINT_OP_DEF(op_literal) \
  Big_Integer op_literal(const Big_Integer &other)
#define __BIGINT_OP_LOGIC_DEF(op_literal) \
  bool op_literal(const Big_Integer &other)
#define __BIGINT_OPERATOR_DEF(op, alter_id) \
  Big_Integer operator op(const Big_Integer &other) { return alter_id(other); }
#define __BIGINT_OPERATOR_LOGIC_DEF(op, alter_id) \
  bool operator op(Big_Integer &other) { return alter_id(other); }

class Big_Integer {
 private:
  std::vector<uint64_t> values;
  std::string values_str;
  Big_Integer(std::vector<uint64_t> values) : values(values) {}

 public:
  Big_Integer(size_t digitSize) {
    values.reserve(digitSize / 64 + digitSize % 64 == 0 ? 1 : 0);
  }

  Big_Integer(std::string value);
  std::string to_string(Big_Integer &other);

  static const Big_Integer ONE;
  __BIGINT_OP_DEF(add);
  __BIGINT_OP_DEF(min);
  __BIGINT_OP_DEF(mul);
  __BIGINT_OP_DEF(div);

  __BIGINT_OP_LOGIC_DEF(inc);
  __BIGINT_OP_LOGIC_DEF(dec);

  __BIGINT_OP_LOGIC_DEF(_not);
  __BIGINT_OP_LOGIC_DEF(_and);
  __BIGINT_OP_LOGIC_DEF(_or);
  __BIGINT_OP_LOGIC_DEF(_xor);

  __BIGINT_OP_LOGIC_DEF(gt);
  __BIGINT_OP_LOGIC_DEF(gteq);
  __BIGINT_OP_LOGIC_DEF(lt);
  __BIGINT_OP_LOGIC_DEF(lteq);
  __BIGINT_OP_LOGIC_DEF(equal);
  __BIGINT_OP_LOGIC_DEF(noteq);
  __BIGINT_OP_LOGIC_DEF(andand);
  __BIGINT_OP_LOGIC_DEF(oror);

  __BIGINT_OPERATOR_DEF(+, add)
  __BIGINT_OPERATOR_DEF(-, min);
  __BIGINT_OPERATOR_DEF(*, mul)
  __BIGINT_OPERATOR_DEF(/, div)

  // special case
  Big_Integer operator++(int) { return add(ONE); }
  Big_Integer operator--(int) { return min(ONE); }
  Big_Integer operator!() { return values_str.empty() || values_str == "0"; }

  __BIGINT_OPERATOR_LOGIC_DEF(&, _and)
  __BIGINT_OPERATOR_LOGIC_DEF(|, _or)
  __BIGINT_OPERATOR_LOGIC_DEF(^, _xor)

  __BIGINT_OPERATOR_LOGIC_DEF(>, gt);
  __BIGINT_OPERATOR_LOGIC_DEF(>=, gteq);
  __BIGINT_OPERATOR_LOGIC_DEF(<, lt);
  __BIGINT_OPERATOR_LOGIC_DEF(<=, lteq);
  __BIGINT_OPERATOR_LOGIC_DEF(==, equal);
  __BIGINT_OPERATOR_LOGIC_DEF(!=, noteq);
  __BIGINT_OPERATOR_LOGIC_DEF(&&, andand);
  __BIGINT_OPERATOR_LOGIC_DEF(||, oror);
};

#endif  // BIG_INT_HXX

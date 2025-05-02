template <typename T>
T move(T&& val) {
  return static_cast<T&&>(val);
}

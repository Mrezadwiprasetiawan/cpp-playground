#ifndef _MATRIX_HXX_
#define _MATRIX_HXX_

#include <cassert>
#include <custom_trait.hxx>
#include <initializer_list>
#include <vector>

namespace l3d {

#define Mat(size)                                                        \
  template <typename T, ifel_trait_t<is_fp<T>, float> = 0>               \
  class Mat##size {                                                      \
    T val[size * size];                                                  \
                                                                         \
   public:                                                               \
    Mat##size() : val() {}                                               \
    Mat##size(T (&val)[size * size]) : val(val) {}                       \
    Mat##size(const std::initializer_list<T> &val) { set_element(val); } \
    Mat##size(T (&val)[size][size]) {                                    \
      for (int i = 0; i < size; ++i)                                     \
        for (int j = 0; j < 3; ++j) this->val[3 * i + j] = val[i][j];    \
    }                                                                    \
                                                                         \
    /* ===== Getter and Setter =====*/                                   \
    T get_matrix_as_array() const { return val; }                        \
    std::vector<T> get_matrix_as_vector() const {                        \
      return std::vector<T>(val);                                        \
    }                                                                    \
    void set_element(T (&val)[size * size]) { this->val = val; }         \
    void set_element(const std::vector<T> &val) {                        \
      assert(val.size() == size * size);                                 \
      for (int i = 0; i < size * size; ++i) this->val = val;             \
    }                                                                    \
    void set_element(const std::initializer_list<T> &val) {              \
      assert(val.size() == size * size);                                 \
      for (int i = 0; i < size * size; ++i) this->val[i] = val[i];       \
    }                                                                    \
                                                                         \
    /* ===== Fungsional ===== */                                         \
    T dot_product(const Mat##size &other);                       \
    Mat##size operator*(T scalar);                                       \
    Mat##size operator*(const Mat##size &other);                         \
    Mat##size operator+(const Mat##size &other);                         \
    Mat##size operator-(const Mat##size other);                          \
    T determinant();                                                     \
    Mat##size transpose();                                               \
    Mat##size inverse();                                                 \
  }

Mat(3);
Mat(4);
#undef Mat

}  // namespace l3d

#endif

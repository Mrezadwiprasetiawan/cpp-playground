#pragma once

#include <custom_trait.hxx>
#include <vector>

namespace l3d {

template <typename T, std::size_t N>
class Vec {
  private:
  T value[N];
  public:
  Vec() { for(int i = 0; i< N; ++i) value[i] = 0; }

  Vec(std::initializer_list<T> list) {
    assert(list.size()==N);
    auto it = list.begin();
    for(int i=0; i< N ; ++i, ++it) value[i] = *it;
  }

  Vec(T (&arr)[N]) { for (int i = 0; i < N; ++i) value[i] = arr[i]; }

  T &operator[](std::size_t i) { return value[i]; }
  const T &operator[](std::size_t i) const { return value[i]; }
  T *data(){ return value; }
};

template <typename T>
using Vec3 = Vec<T, 3>;
template <typename T>
using Vec4 = Vec<T, 4>;
using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;

template <typename T, ifel_trait_t<is_fp<T>, float> = 0>
struct Vertex {
  Vec3<T> pos;

  Vertex(T x = 0, T y = 0, T z = 0) : pos({x, y, z}) {}
  Vertex(std::initializer_list<T> &v) {
    assert(v.size() == 3);
    auto it = v.begin();
    for (int i = 0; i < 3; ++i, ++it) pos[i] = *it;
  }
  Vertex(const Vec3<T> &vec) : pos(vec) {}
};

template <typename T, ifel_trait_t<is_int<T>, int> = 0>
struct Face {
  Vec3<T> idx;

  Face(T i0 = 0, T i1 = 1, T i2 = 2) : idx({i0, i1, i2}) {}
  Face(const Vec3<T> &vec) : idx(vec) {}

  template <typename V>
  bool is_reachable(const std::vector<Vertex<V>> &vertices) {
    const size_t max = vertices.size() - 1;
    return idx[0] >= 0 && idx[0] <= max && idx[1] >= 0 && idx[1] <= max &&
           idx[2] >= 0 && idx[2] <= max;
  }

  Vec3<T> to_vec() const { return idx; }
};

}  // namespace l3d

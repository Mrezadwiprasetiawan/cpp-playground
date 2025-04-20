#ifndef _VERTEX_HXX_
#define _VERTEX_HXX_

#include <cstddef>
#include <cstdio>
#include <custom_trait.hxx>
#include <vector>

namespace l3d {

template <typename T>
struct Vec3 {
  T v0, v1, v2;
  Vec3(T v0 = 0, T v1 = 0, T v2 = 0) : v0(v0), v1(v1), v2(v2) {}
};

template <typename T, ifel_trait_t<is_fp<T>, float> = 0>
struct Vertex {
  T x, y, z;
  Vertex(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}
  Vertex(const Vec3<T> &vec3) : x(vec3.v0), y(vec3.v1), z(vec3.v2) {}
  Vec3<T> to_vec3() const { return Vec3(x, y, z); }
};

template <typename T, ifel_trait_t<is_int<T>, size_t> = 0>
struct Face {
  T i0, i1, i2;

  Face(T i0 = 0, T i1 = 1, T i2 = 2) : i0(i0), i1(i1), i2(i2) {}
  Face(const Vec3<T> &vec3) : i0(vec3.v0), i1(vec3.v1), i2(vec3.v2) {}

  template <typename V>
  bool is_reachable(const std::vector<Vertex<V>> &vertices) {
#define range(coord, max) coord<0 || coord> max
    const size_t max = vertices.size() - 1;
    if (range(this->i0, max) || range(this->i1, max) || range(this->i2, max))
      return false;
#undef range
    return true;
  }

  Vec3<T> to_vec3() const { return Vec3(i0, i1, i2); }
};

}  // namespace l3d
#endif

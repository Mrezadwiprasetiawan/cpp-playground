#pragma once

#include <cassert>
#include <matrix.hxx>
#include <vec.hxx>

namespace l3d {
template <typename V, typename F>
class Obj3D {
  std::vector<Vec3<V>> vertices;
  std::vector<Vec3<F>> face_indices;
  std::vector<Vec3<V>> renew_vertices;
  std::vector<Vec3<V>> normals;
  Mat<V, 4> modelMatrix;

  // update vertices
  void update_vertices(int line) {
    for (size_t i = 0; i < face_indices.size(); ++i) {
      Vec3 f_i = face_indices[i];
      assert(f_i.x > 0 && f_i.x < vertices.size());
      assert(f_i.y > 0 && f_i.y < vertices.size());
      assert(f_i.z > 0 && f_i.z < vertices.size());
      renew_vertices.push_back(vertices[f_i.x]);
      renew_vertices.push_back(vertices[f_i.y]);
      renew_vertices.push_back(vertices[f_i.z]);
    }
  }

 public:
  /*
   * otomatis memperbarui vertices tapi originalnya tidak dihapus agar lebih
   * mudah diambil nanti
   */
  Obj3D(std::vector<Vec3<V>> vertices, std::vector<Vec3<F>> face_indices)
      : vertices(vertices), face_indices(face_indices) {
    update_vertices();
    modelMatrix.set_identity();
  }

  // ===== Fungsional ======
  // rotasi
  void rotate_local(V degree, V x, V y, V z);
  void rotate_global(V degree, V x, V y, V z);
  void translate_local(V x, V y, V z);
  void translate_global(V x, V y, V z);

  // ===== Getter and Setter =====
  // getter
  std::vector<Vec3<F>> get_face_index() const { return face_indices; }
  std::vector<Vec3<V>> get_default_vertices() const { return vertices; }
  std::vector<Vec3<V>> get_processed_vertices() const { return renew_vertices; }
  // setter
  void update_vertices(std::vector<Vec3<V>> vertices) {
    this->vertices = vertices;
    update_vertices();
  }
  void update_face(std::vector<Vec3<V>> face_indices) {
    this->face_indices = face_indices;
    update_vertices();
  }
  void update_face_vertices(std::vector<Vec3<V>> vertices,
                            std::vector<Vec3<F>> face_indices) {
    this->vertices = vertices;
    this->face_indices = face_indices;
    update_vertices();
  }
};
}  // namespace l3d

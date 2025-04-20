#ifndef _OBJECT_3D_HXX_
#define _OBJECT_3D_HXX_

#include <cassert>
#include <vertex.hxx>

namespace l3d {
template <typename V, typename F>
class Object3D {
  std::vector<Vertex<V>> vertices;
  std::vector<Face<F>> face_indices;
  std::vector<Vertex<V>> renew_vertices;
  std::vector<Vertex<V>> normals;

  // update vertices
  void update_vertices(int line) {
    for (size_t i = 0; i < face_indices.size(); ++i) {
      Face f_i = face_indices[i];
      assert(f_i.is_reachable(vertices));
      renew_vertices.push_back(vertices[f_i.i0]);
      renew_vertices.push_back(vertices[f_i.i1]);
      renew_vertices.push_back(vertices[f_i.i2]);
    }
  }

 public:
  /*
   * otomatis memperbarui vertices tapi originalnya tidak dihapus agar lebih
   * mudah diambil nanti
   */
  Object3D(std::vector<Vertex<V>> vertices, std::vector<Face<F>> face_indices)
      : vertices(vertices), face_indices(face_indices) {
    update_vertices();
  }

  // ===== Fungsional ======
  // rotasi
  void rotate_local(V degree, V x, V y, V z);
  void rotate_global(V degree, V x, V y, V z);
  void translate_local(V x, V y, V z);
  void translate_global(V x, V y, V z);


  // ===== Getter and Setter =====
  // getter
  std::vector<Face<F>> get_face_index() const { return face_indices; }
  std::vector<Vertex<V>> get_default_vertices() const { return vertices; }
  std::vector<Vertex<V>> get_processed_vertices() const {
    return renew_vertices;
  }
  // setter
  void update_vertices(std::vector<Vertex<V>> vertices) {
    this->vertices = vertices;
    update_vertices();
  }
  void update_face(std::vector<Vertex<V>> face_indices) {
    this->face_indices = face_indices;
    update_vertices();
  }
  void update_face_vertices(std::vector<Vertex<V>> vertices,
                            std::vector<Face<F>> face_indices) {
    this->vertices = vertices;
    this->face_indices = face_indices;
    update_vertices();
  }
};
}  // namespace l3d

#endif

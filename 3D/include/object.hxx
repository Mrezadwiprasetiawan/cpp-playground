/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/


#pragma once

#include <concepts>
#include <cstddef>
#include <matrix.hxx>
#include <vec.hxx>

namespace l3d {
using namespace Linear;
#define TEMPLATE_OBJ template <std::floating_point FP, std::integral I>

TEMPLATE_OBJ
class Debugger;  // forward declaration

/** Template class for an Interface of 3D Object
 * This class is used to represent an object in 3D space, with translation and rotation capabilities.
 * It can be used as a base class for more specific objects like Cube, Sphere, etc.
 * @param FP Floating point type, e.g., float or double
 * @param I Integral type, e.g., int or size_t
 */
TEMPLATE_OBJ
class AbstractObject {
  Mat<FP, 3>                    modelMat, QCurrentRotationMatrix;
  Vec3<FP>                      pos;
  std::vector<Vec3<FP>>         vertices, newVertices, texture;
  mutable std::vector<Vec3<FP>> normals;  // i want this can be evaluated after this method call
  std::vector<Vec3<I>>          faceIndices;
  friend class Debugger<FP, I>;

  // update vertices
  void update_vertices() {
    newVertices.clear();  // clear biar ga menumpuk
    for (size_t i = 0; i < faceIndices.size(); ++i) {
      Vec3<I> f_i = faceIndices[i];
      assert(f_i.x() >= 0 && f_i.x() < vertices.size());
      assert(f_i.y() >= 0 && f_i.y() < vertices.size());
      assert(f_i.z() >= 0 && f_i.z() < vertices.size());
      newVertices.push_back(vertices[f_i.x()]);
      newVertices.push_back(vertices[f_i.y()]);
      newVertices.push_back(vertices[f_i.z()]);
    }
  }

 public:
  // ===== Fungsional ======
  // rotasi
  /** Hanya merubah matriks Quaternion agar matriks model tidak perlu dihitung
   * trus menerus setiap kali set rotasi
   * @param xyz vektor rotasi
   * @param rad sudut rotasi dalam radian
   * @note rotasi lokal akan mengubah posisi objek relatif terhadap dirinya sendiri
   */
  void rotate_local(Vec3<FP> xyz, FP rad) noexcept {
    Mat3<FP> Qnew           = QUATERNION_MATRIX(xyz, rad);
    QCurrentRotationMatrix *= Qnew;
  }

  /** Hanya merubah matriks Quaternion agar matriks model tidak perlu dihitung
   * trus menerus setiap kali set rotasi
   * @param xyz vektor rotasi
   * @param rad sudut rotasi dalam radian
   * @note rotasi global akan mengubah posisi objek relatif terhadap global axis
   */
  void rotate_global(Vec3<FP> xyz, FP rad) noexcept {
    Mat3<FP> Qnew          = QUATERNION_MATRIX(xyz, rad);
    QCurrentRotationMatrix = Qnew * QCurrentRotationMatrix;
  }

  /** Hanya merubah vector posisi objek bukan matrix untuk efisiensi
   * @param xyz vektor translasi
   * @note translasi lokal akan mengubah posisi objek relatif terhadap dirinya sendiri, terpengaruh rotasi objek
   */
  void translate_local(Vec3<FP> xyz) noexcept { pos += QCurrentRotationMatrix * xyz; }
  /** Hanya merubah vector posisi objek bukan matrix untuk efisiensi
   * @param xyz vektor translasi
   * @note translasi global akan mengubah posisi objek relatif terhadap global axis, tidak terpengaruh rotasi objek
   */
  void translate_global(Vec3<FP> xyz) noexcept { pos += xyz; }
  // ===== Getter and Setter =====
  // @return translation vector
  Vec3<FP> get_translation() const { return pos; }
  // @return rotation quaternion matrix
  Mat3<FP> get_rotation_matrix() const { return QCurrentRotationMatrix; }
  // @return model matrix, model matrix dihitung disini
  Mat4<FP> get_model_matrix() const {
    Mat4<FP> res = mat3_to_mat4(QCurrentRotationMatrix * modelMat);
    res.set_element(3, pos.x());
    res.set_element(7, pos.y());
    res.set_element(11, pos.z());
    return res;
  }
  // @return face_indices that has been setted to the object
  std::vector<Vec3<I>> get_face_indices() const { return faceIndices; }
  // @return vertices that has been setted to the object
  std::vector<Vec3<FP>> get_default_vertices() const { return vertices; }
  // @return processed vertices, vertices that has been processed by faceIndices
  std::vector<Vec3<FP>> get_processed_vertices() const { return newVertices; }
  // @return normals of the processed vertices, if not setted, will be calculated from processed vertices
  std::vector<Vec3<FP>> get_normals() const {
    const auto& src = newVertices.empty() ? vertices : newVertices;
    assert(src.size() % 3 == 0 && "Need a multiple of 3 vertices to build triangles.");
    if (!normals.empty() && normals.size() == src.size()) return normals;
    normals.clear();
    for (size_t i = 0; i < src.size(); i += 3) {
      Vec3<FP> n = normalize(cross(src[i + 1] - src[i], src[i + 2] - src[i]));
      normals.push_back(n);
      normals.push_back(n);
      normals.push_back(n);
    }
    return normals;
  }

  /** set vertices and face indices
   * @param vertices vector of vertices that you want to set
   * @note this will update the vertices and face indices, and also update the processed vertices
   * @note if you want to set face indices, use update_face_vertices instead
   */
  void update_vertices(std::vector<Vec3<FP>> vertices) {
    this->vertices = vertices;
    update_vertices();
  }
  /** set face indices
   * @param faceIndices vector of face indices that you want to set
   * @note this will update the face indices, and also update the processed vertices
   * @note if you want to set vertices, use update_vertices instead
   */
  void update_face(std::vector<Vec3<FP>> faceIndices) {
    this->faceIndices = faceIndices;
    update_vertices();
  }
  /** set vertices and face indices
   * @param vertices vector of vertices that you want to set
   * @param faceIndices vector of face indices that you want to set
   * @note this will update the vertices and face indices, and also update the processed vertices
   * @note if you want to set only vertices, use update_vertices instead
   * @note if you want to set only face indices, use update_face instead
   */
  void update_face_vertices(std::vector<Vec3<FP>> vertices, std::vector<Vec3<I>> faceIndices) {
    this->vertices    = vertices;
    this->faceIndices = faceIndices;
    update_vertices();
  }

  // this function just a starting point cause i dont implement other support
  void set_texture(std::vector<Vec3<FP>> texture) { this->texture = texture; }

  AbstractObject()                                        = default;
  AbstractObject(const AbstractObject<FP, I>&)            = default;
  AbstractObject& operator=(const AbstractObject<FP, I>&) = default;
  ~AbstractObject()                                       = default;

 protected:
  /**
   * otomatis memperbarui vertices tapi originalnya tidak dihapus agar lebih
   * mudah diambil nanti
   */
  explicit AbstractObject(std::vector<Vec3<FP>> vertices, std::vector<Vec3<I>> faceIndices) : vertices(vertices), faceIndices(faceIndices) {
    update_vertices();
    QCurrentRotationMatrix.set_identity();
    modelMat.set_identity();
  }

  /** otomatis memperbarui vertices tapi originalnya tidak dihapus agar lebih
   * mudah diambil nanti
   * @param vertices vector of vertices that you want to set
   * @note face indices will be empty, so you need to set it later, or not using face indices at all
   * @note this will update the processed vertices
   */
  explicit AbstractObject(std::vector<Vec3<FP>> vertices) : vertices(vertices) {
    QCurrentRotationMatrix.set_identity();
    modelMat.set_identity();
  }

  /** otomatis memperbarui vertices tapi originalnya tidak dihapus agar lebih
   * mudah diambil nanti
   * @param vertices initializer list of vertices that you want to set
   * @note face indices will be empty, so you need to set it later, or not using face indices at all
   * @note this will update the processed vertices
   * @assert vertices.size() % 3 == 0, so that it can be used to build triangles
   */
  explicit AbstractObject(const std::initializer_list<FP>& vertices) {
    assert(vertices.size() % 3 == 0);
    for (auto it = vertices.begin(); it != vertices.end(); it += 3) this->vertices.push_back({*it, *(it + 1), *(it + 2)});
  }
  void set_model_matrix(const Mat3<FP> modelMat) { this->modelMat = modelMat; }

  std::vector<Vec3<FP>>& get_texture() { return this->texture; }

 public:
  // move constuctor
  explicit AbstractObject(AbstractObject&& other) noexcept
      : modelMat(std::move(other.modelMat)),
        QCurrentRotationMatrix(std::move(other.QCurrentRotationMatrix)),
        pos(std::move(other.pos)),
        vertices(std::move(other.vertices)),
        faceIndices(std::move(other.faceIndices)),
        newVertices(std::move(other.newVertices)),
        normals(std::move(other.normals)) {}

  // move assignment
  AbstractObject& operator=(AbstractObject&& other) noexcept {
    if (this != &other) {
      modelMat               = std::move(other.modelMat);
      QCurrentRotationMatrix = std::move(other.QCurrentRotationMatrix);
      pos                    = std::move(other.pos);
      vertices               = std::move(other.vertices);
      faceIndices            = std::move(other.faceIndices);
      newVertices            = std::move(other.newVertices);
      normals                = std::move(other.normals);
    }
    return *this;
  }
};

TEMPLATE_OBJ
class Camera {
  Vec3<FP> eye;
  Vec3<FP> center;
  Vec3<FP> up;
  Mat3<FP> QCurrentRotationMatrix;

 public:
  explicit Camera(Vec3<FP> eye = {0, 0, 0}, Vec3<FP> center = {0, 0, -1}, Vec3<FP> up = {0, 1, 0}) : eye(eye), center(center), up(up) {
    QCurrentRotationMatrix.set_identity();
  }

  Mat4<FP> get_view_matrix() const { return VIEW_MATRIX(eye, center, up); }

  Vec3<FP> get_position() const { return eye; }
  Vec3<FP> get_center() const { return center; }
  Vec3<FP> get_up() const { return up; }

  void translate_global(const Vec3<FP>& offset) {
    eye    += offset;
    center += offset;
  }

  /* this function will move the focus of the camera
   * @param offset offset to move the focus, in global coordinates
   * @note this will not change the eye position, only the center position
   * @note this is useful for moving the focus of the camera without changing the eye position
   */
  void move_focus_global(const Vec3<FP>& offset) { center += offset; }

  /** Move the focus of the camera in local coordinates
   * @param offset offset to move the focus, in local coordinates
   * @note this will not change the eye position, only the center position
   * @note this is useful for moving the focus of the camera without changing the eye position
   */
  void move_focus_local(const Vec3<FP>& offset) {
    Vec3<FP> dir   = normalize(center - eye);       // forward
    Vec3<FP> right = normalize(cross(dir, up));     // right
    Vec3<FP> newUp = normalize(cross(right, dir));  // corrected up

    Vec3<FP> localOffset  = offset.x() * right + offset.y() * newUp + offset.z() * dir;
    center               += localOffset;
  }

  /** Rotate object around local axis
   * @param axis axis of rotation, should be normalized
   * @param rad angle of rotation in radians
   * @note this will rotate the camera around the local axis, not the global axis
   * @note this will also update the center and up vector of the camera
   * @note this will not change the eye position, only the center and up vector
   */
  void rotate_local(Vec3<FP> axis, FP rad) {
    Mat3<FP> Qnew           = QUATERNION_MATRIX(axis, rad);
    QCurrentRotationMatrix *= Qnew;

    Vec3<FP> forward = center - eye;
    forward          = Qnew * forward;
    up               = Qnew * up;
    center           = eye + forward;
  }

  /** Rotate object around global axis
   * @param axis axis of rotation, should be normalized
   * @param rad angle of rotation in radians
   * @note this will rotate the camera around the global axis, not the local axis
   * @note this will also update the center and up vector of the camera
   * @note this will not change the eye position, only the center and up vector
   */
  void rotate_global(Vec3<FP> axis, FP rad) {
    Mat3<FP> Qnew          = QUATERNION_MATRIX(axis, rad);
    QCurrentRotationMatrix = Qnew * QCurrentRotationMatrix;

    Vec3<FP> forward = center - eye;
    forward          = Qnew * forward;
    up               = Qnew * up;
    center           = eye + forward;
  }
};

TEMPLATE_OBJ
class Object2D : public AbstractObject<FP, I> {
 protected:
  explicit Object2D<FP, I>(std::vector<Vec3<FP>> vertices, std::vector<Vec3<I>> faceIndices) : AbstractObject<FP, I>(vertices, faceIndices) {}
  explicit Object2D(std::vector<Vec3<FP>> vertices) : AbstractObject<FP, I>(vertices) {}
  explicit Object2D(const std::initializer_list<FP>& vertices) : AbstractObject<FP, I>(vertices) {}
  explicit Object2D(Object2D&& other) noexcept : AbstractObject<FP, I>(other) {}
  Object2D& operator=(Object2D&& other) noexcept {
    AbstractObject<FP, I>::operator=(std::move(other));
    return *this;
  }

 public:
  Object2D()                = delete;
  Object2D(const Object2D&) = default;
  /* pos at left top*/
  explicit Object2D(Vec3<FP> pos, FP width, FP height)
      : AbstractObject<FP, I>({{0, 0, 0}, {width, 0, 0}, {width, height, 0}, {0, height, 0}}, {{0, 1, 2}, {2, 3, 0}}) {
    AbstractObject<FP, I>::translate_global({pos[0], pos[1], 0});
  }

  /* bind to the camera so the object will not be moving while camera moved
   * @param camera Camera object that will be used to bind the object
   * @note this will set the model matrix to the inverse of the camera view matrix multiplied by the current model matrix
   * @note this is useful for 2D objects that should not move while the camera is moving, like UI elements
   */
  void bind_to_camera(const Camera<FP, I>& camera) {
    AbstractObject<FP, I>::set_model_matrix(camera.get_view_matrix().inverse() * AbstractObject<FP, I>::get_model_matrix());
  }
};

/* This is the main class for 3D object
 */
TEMPLATE_OBJ
class Object3D : public AbstractObject<FP, I> {
 public:
  Object3D()                = default;
  Object3D(const Object3D&) = default;
  explicit Object3D(std::vector<Vec3<FP>> vertices, std::vector<Vec3<I>> faceIndices) : AbstractObject<FP, I>(vertices, faceIndices) {}
  explicit Object3D(std::vector<Vec3<FP>> vertices) : AbstractObject<FP, I>(vertices) {}
  explicit Object3D(const std::initializer_list<FP>& vertices) : AbstractObject<FP, I>(vertices) {}

  explicit Object3D(Object3D&& other) noexcept : AbstractObject<FP, I>(std::move(other)) {}
  Object3D& operator=(Object3D&& other) noexcept {
    AbstractObject<FP, I>::operator=(std::move(other));
    return *this;
  }
};

#undef TEMPLATE_OBJ

}  // namespace l3d
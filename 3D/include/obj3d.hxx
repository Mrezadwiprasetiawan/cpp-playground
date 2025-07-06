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

#include <cassert>
#include <concepts>
#include <initializer_list>
#include <matrix.hxx>
#include <vec.hxx>

namespace l3d {
using namespace Linear;
// FP = floating point I = integer
template <std::floating_point FP = float, std::integral I = short>
class Obj3D {
  Mat<FP, 3>            modelMat;
  Mat<FP, 3>            QcurrMat;
  Vec3<FP>              pos;
  std::vector<Vec3<FP>> vertices;
  std::vector<Vec3<I>>  faceIndices;
  std::vector<Vec3<FP>> newVertices;
  std::vector<Vec3<FP>> normals;

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
  // menghindari copy constructor
  explicit Obj3D(const Obj3D&) = delete;
  /*
   * otomatis memperbarui vertices tapi originalnya tidak dihapus agar lebih
   * mudah diambil nanti
   */
  explicit Obj3D(std::vector<Vec3<FP>> vertices, std::vector<Vec3<I>> faceIndices) : vertices(vertices), faceIndices(faceIndices) {
    update_vertices();
    QcurrMat.set_identity();
    modelMat.set_identity();
  }

  explicit Obj3D(std::vector<Vec3<FP>> vertices) noexcept : vertices(vertices) {
    QcurrMat.set_identity();
    modelMat.set_identity();
  }

  explicit Obj3D(const std::initializer_list<FP>& vertices) {
    assert(vertices.size() % 3 == 0);
    for (auto it = vertices.begin(); it != vertices.end(); it += 3) this->vertices.push_back({*it, *(it + 1), *(it + 2)});
  }

  // move
  explicit Obj3D(Obj3D&& other) noexcept
      : modelMat(std::move(other.modelMat)),
        QcurrMat(std::move(other.QcurrMat)),
        pos(std::move(other.pos)),
        vertices(std::move(other.vertices)),
        faceIndices(std::move(other.faceIndices)),
        newVertices(std::move(other.newVertices)),
        normals(std::move(other.normals)) {}

  // move assignment
  Obj3D& operator=(Obj3D&& other) noexcept {
    if (this != &other) {
      modelMat    = std::move(other.modelMat);
      QcurrMat    = std::move(other.QcurrMat);
      pos         = std::move(other.pos);
      vertices    = std::move(other.vertices);
      faceIndices = std::move(other.faceIndices);
      newVertices = std::move(other.newVertices);
      normals     = std::move(other.normals);
    }
    return *this;
  }

  // ===== Fungsional ======
  // rotasi
  /* Hanya merubah matriks Quaternion agar matriks model tidak perlu dihitung
   * trus menerus setiap kali set rotasi
   */
  void rotate_local(Vec3<FP> xyz, FP rad) noexcept {
    Mat3<FP> Qnew  = QUATERNION_MATRIX(xyz, rad);
    QcurrMat      *= Qnew;
  }

  void rotate_global(Vec3<FP> xyz, FP rad) noexcept {
    Mat3<FP> Qnew = QUATERNION_MATRIX(xyz, rad);
    QcurrMat      = Qnew * QcurrMat;
  }

  void translate_local(Vec3<FP> xyz) noexcept { pos += QcurrMat * xyz; }
  void translate_global(Vec3<FP> xyz) noexcept { pos += xyz; }

  // ===== Getter and Setter =====
  // getter
  Vec3<FP> get_translation() const { return pos; }
  Mat3<FP> get_rotation_matrix() const { return QcurrMat; }
  Mat4<FP> get_model_matrix() const {
    Mat4<FP> res = mat3_to_mat4(QcurrMat * modelMat);
    res.set_element(3, pos.x());
    res.set_element(7, pos.y());
    res.set_element(11, pos.z());
    return res;
  }

  std::vector<Vec3<I>>  get_face_indices() const { return faceIndices; }
  std::vector<Vec3<FP>> get_default_vertices() const { return vertices; }
  std::vector<Vec3<FP>> get_processed_vertices() const { return newVertices; }
  std::vector<Vec3<FP>> get_normals() {
    if (normals.empty()) {
      if (newVertices.empty()) {
        assert(vertices.size() % 3 == 0 &&
               "cant use vertices if count != 0 (mod3), needed for build triangles.\nupdate vertices, or setup indices if you want use vertices again");
        for (size_t i = 0; i < vertices.size(); i += 3) normals.push_back(normalize(cross(vertices[i + 2] - vertices[i], vertices[i + 1], vertices[i])));
      } else
        for (size_t i = 0; i < newVertices.size(); i += 3)
          normals.push_back(normalize(cross(newVertices[i + 2] - newVertices[i], newVertices[i + 1], newVertices[i])));
    }
    return normals;
  }
  // setter
  void update_vertices(std::vector<Vec3<FP>> vertices) {
    this->vertices = vertices;
    update_vertices();
  }
  void update_face(std::vector<Vec3<FP>> faceIndices) {
    this->faceIndices = faceIndices;
    update_vertices();
  }
  void update_face_vertices(std::vector<Vec3<FP>> vertices, std::vector<Vec3<I>> faceIndices) {
    this->vertices    = vertices;
    this->faceIndices = faceIndices;
    update_vertices();
  }

 protected:
  void set_model_matrix(const Mat3<FP> modelMat) { this->modelMat = modelMat; }
};
}  // namespace l3d

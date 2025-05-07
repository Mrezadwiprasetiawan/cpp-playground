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
#include <matrix.hxx>
#include <vec.hxx>

namespace l3d {
// FP = floating point I = integer
template <typename FP, typename I>
class Obj3D {
  Mat<FP, 3> modelMat;
  Mat<FP, 3> QcurrMat;
  Vec3<FP> pos;
  std::vector<Vec3<FP>> vertices;
  std::vector<Vec3<I>> faceIndices;
  std::vector<Vec3<FP>> newVertices;
  std::vector<Vec3<FP>> normals;

  // update vertices
  void update_vertices() {
    for (size_t i = 0; i < faceIndices.size(); ++i) {
      Vec3 f_i = faceIndices[i];
      assert(f_i.x() >= 0 && f_i.x() < vertices.size());
      assert(f_i.y() >= 0 && f_i.y() < vertices.size());
      assert(f_i.z() >= 0 && f_i.z() < vertices.size());
      newVertices.push_back(vertices[f_i.x()]);
      newVertices.push_back(vertices[f_i.y()]);
      newVertices.push_back(vertices[f_i.z()]);
    }
  }

 public:
  /*
   * otomatis memperbarui vertices tapi originalnya tidak dihapus agar lebih
   * mudah diambil nanti
   */
  Obj3D(std::vector<Vec3<FP>> vertices, std::vector<Vec3<I>> faceIndices)
      : vertices(vertices), faceIndices(faceIndices) {
    update_vertices();
    QcurrMat.set_identity();
    modelMat.set_identity();
  }

  // ===== Fungsional ======
  // rotasi
  /* Hanya merubah matriks Quaternion agar matriks model tidak perlu dihitung
   * trus menerus setiap kali set rotasi
   */
  void rotate_local(Vec3<FP> xyz, FP rad) {
    Mat3<FP> Qnew = QUATERNION_MATRIX(xyz, rad);
    QcurrMat *= Qnew;
  }

  void rotate_global(Vec3<FP> xyz, FP rad) {
    Mat3<FP> Qnew = QUATERNION_MATRIX(xyz, rad);
    QcurrMat = Qnew * QcurrMat;
  }

  void translate_local(Vec3<FP> xyz) { pos += QcurrMat * xyz; }
  void translate_global(Vec3<FP> xyz) { pos += xyz; }

  // ===== Getter and Setter =====
  // getter
  Vec3<FP> get_translation() { return pos; }
  Mat3<FP> get_rotation_matrix() { return QcurrMat; }
  Mat4<FP> get_model_matrix() {
    Mat4<FP> res = mat3_to_mat4(QcurrMat * modelMat * QcurrMat.inverse());
    res.set_element(3, pos.x());
    res.set_element(7, pos.y());
    res.set_element(11, pos.z());
    return res;
  }

  std::vector<Vec3<I>> get_face_index() const { return faceIndices; }
  std::vector<Vec3<FP>> get_default_vertices() const { return vertices; }
  std::vector<Vec3<FP>> get_processed_vertices() const { return newVertices; }
  // setter
  void update_vertices(std::vector<Vec3<FP>> vertices) {
    this->vertices = vertices;
    update_vertices();
  }
  void update_face(std::vector<Vec3<FP>> faceIndices) {
    this->faceIndices = faceIndices;
    update_vertices();
  }
  void update_face_vertices(std::vector<Vec3<FP>> vertices,
                            std::vector<Vec3<I>> faceIndices) {
    this->vertices = vertices;
    this->faceIndices = faceIndices;
    update_vertices();
  }
};
}  // namespace l3d

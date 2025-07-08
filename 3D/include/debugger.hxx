#pragma once

#include <concepts>
#include <iostream>
#include <object.hxx>
#include <ostream>
#include <utility>
#include <vector>

namespace l3d {

template <std::floating_point FP, std::integral I>
class Debugger {
  std::vector<std::pair<std::string, AbstractObject<FP, I>>> debugged;

 public:
  Debugger() = default;

  explicit Debugger(const std::vector<std::pair<std::string, AbstractObject<FP, I>>>& debuggedObjects) : debugged(debuggedObjects) {}

  const auto& get_debugged_objects() const { return debugged; }

  void add_object(const std::string& name, const AbstractObject<FP, I>& obj) { debugged.emplace_back(name, obj); }

  void run() {
    for (const auto& [name, obj] : debugged) {
      std::cout << "== Object Name: " << name << " ==\n";

      std::cout << "Vertices:\n";
      for (const auto& vertex : obj.get_default_vertices()) std::cout << "\t" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << "\n";

      std::cout << "Processed Vertices:\n";
      for (const auto& vertex : obj.get_processed_vertices()) std::cout << "\t" << vertex.x() << ", " << vertex.y() << ", " << vertex.z() << "\n";

      std::cout << "Face Indices:\n";
      for (const auto& face : obj.get_face_indices()) std::cout << "\t" << face.x() << ", " << face.y() << ", " << face.z() << "\n";

      std::cout << "Normals:\n";
      for (const auto& normal : obj.get_normals()) std::cout << "\t" << normal.x() << ", " << normal.y() << ", " << normal.z() << "\n";

      std::cout << "Rotation Matrix:\n";
      for (int i = 0; i < 3; ++i) {
        std::cout << "\t";
        Vec3<FP> row = obj.get_rotation_matrix()[i];
        std::cout << row.x() << ", " << row.y() << ", " << row.z() << "\n";
      }

      std::cout << "Model Matrix:\n";
      auto model = obj.get_model_matrix();
      for (int i = 0; i < 4; ++i) {
        std::cout << "\t";
        Vec4<FP> row = model[i];
        std::cout << row.x() << ", " << row.y() << ", " << row.z() << ", " << row.w() << "\n";
      }

      std::cout << "\n========================================\n";
    }
  }
};

}  // namespace l3d

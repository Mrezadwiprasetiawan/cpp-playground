#pragma once

#include <cstdio>
#include <fstream>
#include <object.hxx>
#include <sstream>
#include <stdexcept>
#include <vec.hxx>
#include <vector>

l3d::Object3D<float, int> read_from_obj(const std::string& filename) {
  using namespace Linear;
  using namespace l3d;

  l3d::Object3D<float, int> res;

  std::ifstream file(filename);
  if (!file.is_open()) throw std::runtime_error("Failed to open file: " + filename);
  // TODO: implement reading from .obj file
  std::vector<Vec3<float>> vertices;
  std::vector<Vec3<int>>   faceIndices;
  std::string              line;
  while (std::getline(file, line)) {
    if (line.empty() || line[0] == '#') continue;  // skip empty lines and comments
    std::istringstream iss(line);
    std::string        prefix;
    iss >> prefix;
    if (prefix == "v") {
      Vec3<float> vertex;
      float       x, y, z;
      iss >> x >> y >> z;
      vertex = Vec3<float>({x, y, z});
      vertices.push_back(vertex);
    } else if (prefix == "f") {
      Vec3<int> face;
      int       x, y, z;
      iss >> x >> y >> z;
      face = Vec3<int>({x, y, z});
      // Convert to zero-based index
      face -= Vec3<int>({1, 1, 1});
      faceIndices.push_back(face);
    }
  }
  res = Object3D<float, int>(vertices, faceIndices);
  return res;
}

void write_to_obj(const l3d::Object3D<float, int>& obj, const std::string& filename) {
  using namespace Linear;
  using namespace l3d;

  std::ofstream file(filename);
  if (!file.is_open()) throw std::runtime_error("Failed to open file: " + filename);
}
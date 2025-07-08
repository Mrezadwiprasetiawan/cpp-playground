#pragma once

#include <object.hxx>

l3d::Object3D<float, int> read_from_obj(const std::string& filename);

void write_to_obj(const l3d::Object3D<float, int>& obj, const std::string& filename);
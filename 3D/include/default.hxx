#pragma once

#include <concepts>
#include <matrix.hxx>
#include <object.hxx>
#include <vec.hxx>

namespace l3d {
using namespace Linear;

#define TEMPLATE_FLOAT template <std::floating_point FP>
TEMPLATE_FLOAT
struct Triangle {
  Vec3<FP> A, B, C;
};

TEMPLATE_FLOAT
class Cube {};
}  // namespace l3d
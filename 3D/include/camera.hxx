#pragma once

#include <concepts>

#include "obj3d.hxx"

namespace l3d {
template <std::floating_point FP, std::integral I>
class Camera : protected Obj3D<FP, I> {
 public:
  explicit Camera() : Obj3D<FP, I>() {}
};
}  // namespace l3d
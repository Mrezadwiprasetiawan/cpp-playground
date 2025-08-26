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


#include <ffn.hxx>
#include <nn_objects.hxx>

int main() {
  size_t              layer_sizes[] = {1, 1024, 1024, 1};
  NN::ACTIVATION_TYPE act_funcs[]   = {NN::ACTIVATION_TYPE::NONE, NN::ACTIVATION_TYPE::ReLU, NN::ACTIVATION_TYPE::tanh};
  NN::FFN<double>     ffn(layer_sizes, act_funcs);
  return 0;
}
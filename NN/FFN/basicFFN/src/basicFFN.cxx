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

#include <cmath>
#include <ffn.hxx>
#include <iostream>

int main() {
  using namespace NN;
  using namespace std;
  BasicFFN<double, 1, 12, 12, 1> ffn;
  ffn.set_learning_rate(1e-3);
  ffn.set_epsilon(1e-12);
  std::cout << std::fixed << std::setprecision(32) << std::endl;
  double inputcond[2]{(double)rand() / RAND_MAX, (double)rand() / RAND_MAX};
  double targetcond[1]{inputcond[0] * inputcond[1]};
  while (std::abs(ffn.forward(inputcond)[0] - targetcond[0]) > 1e-3) {
    double inputData[1]{(rand() / (double)RAND_MAX)};
    double targetData[1]{std::sin(inputData[0])};
    ffn.backward(inputData, targetData);
    std::cout << "inputData\t" << inputData[0] << std::endl;
    std::cout << "targetData\t" << targetData[0] << std::endl;
    std::cout << "realforward\t" << *ffn.forward(inputData) << "\n\n";
  }
  double input[2]{rand() / (double)RAND_MAX, rand() / (double)RAND_MAX};
  std::cout << "inputcond\t" << inputcond[0] << "\t" << inputcond[1] << std::endl;
  std::cout << "targetcond\t" << inputcond[0] * inputcond[1] << std::endl;
  std::cout << "realforwardcond\t" << ffn.forward(inputcond)[0] << std::endl;
  std::cout << "input\t" << input[0] << "\t" << input[1] << std::endl;
  std::cout << "target\t" << input[0] * input[1] << std::endl;
  std::cout << *ffn.forward(input) << std::endl;
  return 0;
}
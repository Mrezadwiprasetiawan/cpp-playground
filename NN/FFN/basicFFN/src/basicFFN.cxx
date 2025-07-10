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
#include <cstring>
#include <ffn.hxx>
#include <iostream>
#include <map>
#include <string>

int main(int argc, const char **argv) {
  using namespace NN;
  using namespace std;
  BasicFFN<double, 1, 1024, 1024, 1> ffn;
  ffn.set_learning_rate(1e-3);
  ffn.set_epsilon(1e-12);
  ffn.set_weights_filename("wb.bin");
  // ffn.set_debug_mode(true);
  std::cout << std::fixed << std::setprecision(32) << std::endl;
  double inputcond[1]{(double)rand() / RAND_MAX};
  double targetcond[1]{std::sin(inputcond[0])};
  while (std::abs(ffn.forward(inputcond)[0] - targetcond[0]) > 1e-3) {
    double inputData[1]{(rand() / (double)RAND_MAX)};
    double targetData[1]{std::sin(inputData[0])};
    ffn.backward(inputData, targetData);
    std::cout << "inputData\t" << inputData[0] << std::endl;
    std::cout << "targetData\t" << targetData[0] << std::endl;
    std::cout << "realforward\t" << *ffn.forward(inputData) << "\n\n";
    ffn.save_weights();
  }
  double input[2]{rand() / (double)RAND_MAX, rand() / (double)RAND_MAX};
  std::cout << "inputcond\t" << inputcond[0] << std::endl;
  std::cout << "targetcond\t" << targetcond[0] << std::endl;
  std::cout << "realforwardcond\t" << ffn.forward(inputcond)[0] << std::endl;
  std::cout << "input\t" << input[0] << "\t" << input[1] << std::endl;
  std::cout << "target\t" << input[0] * input[1] << std::endl;
  std::cout << *ffn.forward(input) << std::endl;
  return 0;
}
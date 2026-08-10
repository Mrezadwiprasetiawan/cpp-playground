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
#include <ctime>
#include <ffn.hxx>
#include <iostream>
#include <string>

// kumpulan fungsi 1D yang bisa diaproksimasi, tinggal tunjuk salah satunya lewat targetFunc di bawah
namespace TargetFunctions {
double sinFn(double x) { return std::sin(x); }
double cosFn(double x) { return std::cos(x); }
double squareFn(double x) { return x * x; }
double expFn(double x) { return std::exp(x); }
}  // namespace TargetFunctions

int main(int argc, const char **argv) {
  using namespace NN;
  using namespace std;

  // ganti ke TargetFunctions::cosFn / squareFn / expFn kalau mau aproksimasi fungsi lain
  auto targetFunc = TargetFunctions::expFn;

  const size_t batchSize = 32;

  BasicFFN<double, 1, 1024, 1024, 1> ffn;
  ffn.set_learning_rate(1e-3);
  ffn.set_epsilon(1e-12);
  ffn.set_weights_filename("wb.bin");
  std::cout << std::fixed << std::setprecision(32) << std::endl;
  srand(time(0));

  // alokasi buffer batch, ukurannya [batchSize][inputSize] dan [batchSize][outputSize]
  double **inputBatch  = new double *[batchSize];
  double **targetBatch = new double *[batchSize];
  for (size_t i = 0; i < batchSize; ++i) {
    inputBatch[i]  = new double[1];
    targetBatch[i] = new double[1];
  }

  double inputcond[1]{(double)rand() / RAND_MAX};
  double targetcond[1]{targetFunc(inputcond[0])};

  while (std::abs(ffn.forward(inputcond)[0] - targetcond[0]) > 1e-2) {
    for (size_t i = 0; i < batchSize; ++i) {
      inputBatch[i][0]  = (double)rand() / RAND_MAX;
      targetBatch[i][0] = targetFunc(inputBatch[i][0]);
    }
    ffn.backward(inputBatch, targetBatch, batchSize);
    std::cout << "loss\t" << ffn.get_loss() << std::endl;
    ffn.save_weights();
  }

  for (size_t i = 0; i < batchSize; ++i) {
    delete[] inputBatch[i];
    delete[] targetBatch[i];
  }
  delete[] inputBatch;
  delete[] targetBatch;

  double input[]{rand() / (double)RAND_MAX};
  std::cout << "inputcond\t" << inputcond[0] << std::endl;
  std::cout << "targetcond\t" << targetcond[0] << std::endl;
  std::cout << "realforwardcond\t" << ffn.forward(inputcond)[0] << std::endl;
  std::cout << "input\t" << input[0] << std::endl;
  std::cout << "target\t" << targetFunc(input[0]) << std::endl;
  std::cout << *ffn.forward(input) << std::endl;
  return 0;
}
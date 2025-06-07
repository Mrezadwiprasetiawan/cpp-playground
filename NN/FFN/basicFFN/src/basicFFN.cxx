#include <cmath>
#include <ffn.hxx>
#include <iostream>
#include <numbers>

int main() {
  using namespace NN;
  using namespace std;
  BasicFFN<double, 1, 64, 64, 1> ffn;
  // ffn.set_learning_rate(1e-4);

  // ffn.set_epsilon(1e-12) recommended for double

  double input[100][1];
  double x = 0.0f;
  cout << "init data" << endl;
  for (int e = 0; e < 100; ++e) {
    x = 0;
    for (int i = 0; i < 100; x += 1e-2, ++i) {
      input[i][0] = x;
      double sinx[] = {std::sin(x)};
      cout << "train ke" << i << endl;
      ffn.backward(input[i], sinx);
    }
  }

  cout << "after trains" << endl;

  for (int i = 0; i < 100; ++i) {
    cout << "Result for input " << input[i][0] << " = " << ffn.forward(input[i])[0] << endl;
    cout << "Real result = " << std::sin(input[i][0]) << endl;
  }
  return 0;
}
#include <ffn.hxx>
#include <iostream>

int main() {
  using namespace NN;
  using namespace std;
  BasicFFN<float, 3, 128, 128, 1> ffn;
  // ffn.set_epsilon(1e-12) recommended for double
  float inputData[] = {0.2, 0.7, 0.5};  // this is just test if theres no
                                        // runtime error not test for logical
  float (&out)[1] = ffn.forward(inputData);
  cout << out[0] << endl;
  return 0;
}
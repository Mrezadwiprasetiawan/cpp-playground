#include <ffn.hxx>
#include <nn_objects.hxx>

int main() {
  size_t layer_sizes[] = {1, 1024, 1024, 1};
  NN::ACTIVATION_TYPE act_funcs[] = {NN::ACTIVATION_TYPE::NONE, NN::ACTIVATION_TYPE::ReLU, NN::ACTIVATION_TYPE::tanh};
  NN::FFN<double> ffn;
  return 0;
}
#include <ffn.hxx>
#include <nn_objects.hxx>

int main() {
  size_t layer_sizes[] = {1, 1024, 1024, 1};
  ACTIVATION_TYPE act_funcs[] = {ACTIVATION_TYPE::NONE, ACTIVATION_TYPE::ReLU, ACTIVATION_TYPE::tanh};
  FFN<double> ffn(layer_sizes, act_funcs);
  return 0;
}
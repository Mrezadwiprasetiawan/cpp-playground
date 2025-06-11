#include <cmath>
#include <type_traits>
#include <vector>

#define TEMPLATE_FLOAT template <typename FP, typename = std::enable_if_t<std::is_floating_point_v<FP>>>

TEMPLATE_FLOAT
struct WeightBias {
  std::vector<FP> w;
  FP b;
};

TEMPLATE_FLOAT struct Layers {
  size_t size;
  ACTIVATION_TYPE act_func_t;
};

enum ACTIVATION_TYPE { sigmoid, ReLU, tanh };
enum LOSS_TYPE { MAE, MSE, cross_entropy };

#define TEMPLATE_FUNC_FLOAT \
  template <typename FP>    \
  std::enable_if_t<std::is_floating_point_v<FP>>
// Rectified Linear Unit Activation function
TEMPLATE_FUNC_FLOAT ReLU(FP x, FP epsilon) { return x > 0 ? x : epsilon; }
// ReLU derivative function
TEMPLATE_FUNC_FLOAT ReLU_deriv(FP y, FP epsilon) { return y > 0 ? 1 : epsilon; }
// Sigmoid activation function
TEMPLATE_FUNC_FLOAT sigmoid(FP x) { return 1 / (1 + std::exp(-x)); }
// Sigmoid Derivative function
TEMPLATE_FUNC_FLOAT Sigmoid_deriv(FP y) { return y * (1 - y); }
// Tanh activation function
TEMPLATE_FUNC_FLOAT tanh(FP x) { return (std::exp(x) - std::exp(-x)) / (std::exp(x) + std::exp(-x)); }
// Tanh derivative function
TEMPLATE_FUNC_FLOAT tanh_deriv(FP y) { return 1 - y * y; }
// Mean Absolute Error Loss Function
TEMPLATE_FUNC_FLOAT MAE(FP ypred, FP y) { return std::abs(ypred - y); }
// Mean Absolute Error derivative function
TEMPLATE_FUNC_FLOAT MAE_deriv(FP ypred, FP y) {
  FP semi_loss = ypred - y;
  return semi_loss > 0 ? 1 : semi_loss == 0 ? 0 : -1;
}
// Mean Squared Error Loss Function
TEMPLATE_FUNC_FLOAT MSE(FP ypred, FP y) {
  FP semi_loss = ypred - y;
  return 0.5 * semi_loss * semi_loss;
}
// Mean Squared Error derivative function
TEMPLATE_FUNC_FLOAT MSE_deriv(FP ypred, FP y) { return ypred - y; }
// Cross Entropy Loss Function
TEMPLATE_FUNC_FLOAT cross_entropy(FP ypred, FP y, FP epsilon) { return -y * std::log(ypred + epsilon) - (1 - y) * std::log(1 - ypred + epsilon); }
// Cross Entropy dericative function
TEMPLATE_FUNC_FLOAT cross_entropy_deriv(FP ypred, FP y, FP epsilon) { return (ypred - y) / ((ypred + epsilon) * (1 - ypred + epsilon)); }
#include <random>
#include <type_traits>
#include <vector>

template <typename FP, typename = std::enable_if_t<std::is_floating_point_v<FP>, FP>>
struct WeightBias {
  std::vector<FP> w;
  FP b;
};

template <typename FP, typename = std::enable_if_t<std::is_floating_point_v<FP>, FP>>
class FFN {
  std::vector<WeightBias<FP>> layers;
  std::vector<size_t> layerSizes;
  std::random_device rd;
  std::mt19947 gen;

 public:
  FFN(std::vector<size_t> layerSizes, ) : layerSizes(layerSizes) {}
};
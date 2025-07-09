#pragma once
#include <omp.h>

#include <nn_objects.hxx>
#include <random>
#include <vector>

namespace NN {

template <std::floating_point FP>
class FFN {
  std::vector<std::vector<FP>> win, wh1, wh2;
  std::vector<FP>              bin, bh1, bh2;
  ACTIVATION_TYPE              act_s[3];
  LOSS_TYPE                    loss_t;

  inline static FP epsilon = FP(1e-6);
  FP               eta     = FP(1e-2);
  FP               loss_   = FP(0);

  std::random_device           dev;
  std::mt19937_64              gen;
  std::normal_distribution<FP> dis;

  static inline FP activate(FP x, ACTIVATION_TYPE t) {
    switch (t) {
      case ACTIVATION_TYPE::ReLU: return ReLU<FP>(x, epsilon);
      case ACTIVATION_TYPE::sigmoid: return sigmoid<FP>(x);
      case ACTIVATION_TYPE::tanh: return tanh<FP>(x);
      case ACTIVATION_TYPE::NONE: return x;
    }
    return x;
  }
  static inline FP d_activate(FP y, ACTIVATION_TYPE t) {
    switch (t) {
      case ACTIVATION_TYPE::ReLU: return d_ReLU<FP>(y, epsilon);
      case ACTIVATION_TYPE::sigmoid: return d_sigmoid<FP>(y);
      case ACTIVATION_TYPE::tanh: return d_tanh<FP>(y);
      case ACTIVATION_TYPE::NONE: return FP(1);
    }
    return FP(1);
  }

  static inline FP loss(LOSS_TYPE lt, FP y_hat, FP y) {
    switch (lt) {
      case LOSS_TYPE::MAE: return MAE<FP>(y_hat, y);
      case LOSS_TYPE::MSE: return MSE<FP>(y_hat, y);
      case LOSS_TYPE::cross_entropy: return cross_entropy<FP>(y_hat, y, epsilon);
    }
    return FP(0);
  }
  static inline FP d_loss(LOSS_TYPE lt, FP y_hat, FP y, FP eps) {
    switch (lt) {
      case LOSS_TYPE::MAE: return d_MAE<FP>(y_hat, y);
      case LOSS_TYPE::MSE: return d_MSE<FP>(y_hat, y);
      case LOSS_TYPE::cross_entropy: return d_cross_entropy<FP>(y_hat, y, eps);
    }
    return FP(0);
  }

  std::vector<FP> forward(const std::vector<FP>& input) {
    std::vector<FP> h1(wh1.size());
#pragma omp parallel for
    for (size_t j = 0; j < wh1.size(); ++j) {
      FP z = bin[j];
      for (size_t i = 0; i < input.size(); ++i) z += input[i] * win[i][j];
      h1[j] = activate(z, act_s[0], epsilon);
    }

    std::vector<FP> h2(wh2.size());
#pragma omp parallel for
    for (size_t j = 0; j < wh2.size(); ++j) {
      FP z = bh1[j];
      for (size_t i = 0; i < h1.size(); ++i) z += h1[i] * wh1[i][j];
      h2[j] = activate(z, act_s[1], epsilon);
    }

    std::vector<FP> out(bh2.size());
#pragma omp parallel for
    for (size_t j = 0; j < out.size(); ++j) {
      FP z = bh2[j];
      for (size_t i = 0; i < h2.size(); ++i) z += h2[i] * wh2[i][j];
      out[j] = activate(z, act_s[2], epsilon);
    }

    return out;
  }

  void backward(const std::vector<std::vector<FP>>& inputs, const std::vector<std::vector<FP>>& targets) {
    const size_t B = inputs.size();
    if (B == 0) return;

    std::vector<std::vector<FP>> gWin(win.size(), std::vector<FP>(win[0].size(), FP(0)));
    std::vector<std::vector<FP>> gWh1(wh1.size(), std::vector<FP>(wh1[0].size(), FP(0)));
    std::vector<std::vector<FP>> gWh2(wh2.size(), std::vector<FP>(wh2[0].size(), FP(0)));
    std::vector<FP>              gBin(bin.size(), FP(0)), gBh1(bh1.size(), FP(0)), gBh2(bh2.size(), FP(0));

    FP loss_sum = FP(0);

#pragma omp parallel for reduction(+ : loss_sum)
    for (size_t b = 0; b < B; ++b) {
      const auto& input  = inputs[b];
      const auto& target = targets[b];

      std::vector<FP> h1(wh1.size());
      for (size_t j = 0; j < wh1.size(); ++j) {
        FP z = bin[j];
        for (size_t i = 0; i < input.size(); ++i) z += input[i] * win[i][j];
        h1[j] = activate(z, act_s[0], epsilon);
      }

      std::vector<FP> h2(wh2.size());
      for (size_t j = 0; j < wh2.size(); ++j) {
        FP z = bh1[j];
        for (size_t i = 0; i < h1.size(); ++i) z += h1[i] * wh1[i][j];
        h2[j] = activate(z, act_s[1], epsilon);
      }

      std::vector<FP> out(bh2.size());
      for (size_t j = 0; j < out.size(); ++j) {
        FP z = bh2[j];
        for (size_t i = 0; i < h2.size(); ++i) z += h2[i] * wh2[i][j];
        out[j] = activate(z, act_s[2], epsilon);
      }

      FP l = FP(0);
      for (size_t i = 0; i < out.size(); ++i) l += loss(loss_t, out[i], target[i], epsilon);
      loss_sum += l;

      std::vector<FP> delta_out(out.size());
      for (size_t i = 0; i < out.size(); ++i) {
        FP dL        = d_loss(loss_t, out[i], target[i], epsilon);
        FP dact      = d_activate(out[i], act_s[2], epsilon);
        delta_out[i] = dL * dact;
      }

      std::vector<FP> delta_h2(h2.size(), FP(0));
      for (size_t i = 0; i < h2.size(); ++i) {
        for (size_t j = 0; j < out.size(); ++j) delta_h2[i] += wh2[i][j] * delta_out[j];
        delta_h2[i] *= d_activate(h2[i], act_s[1], epsilon);
      }

      std::vector<FP> delta_h1(h1.size(), FP(0));
      for (size_t i = 0; i < h1.size(); ++i) {
        for (size_t j = 0; j < h2.size(); ++j) delta_h1[i] += wh1[i][j] * delta_h2[j];
        delta_h1[i] *= d_activate(h1[i], act_s[0], epsilon);
      }

      for (size_t i = 0; i < win.size(); ++i)
        for (size_t j = 0; j < win[0].size(); ++j) gWin[i][j] += input[i] * delta_h1[j];

      for (size_t j = 0; j < gBin.size(); ++j) gBin[j] += delta_h1[j];

      for (size_t i = 0; i < wh1.size(); ++i)
        for (size_t j = 0; j < wh1[0].size(); ++j) gWh1[i][j] += h1[i] * delta_h2[j];

      for (size_t j = 0; j < gBh1.size(); ++j) gBh1[j] += delta_h2[j];

      for (size_t i = 0; i < wh2.size(); ++i)
        for (size_t j = 0; j < wh2[0].size(); ++j) gWh2[i][j] += h2[i] * delta_out[j];

      for (size_t j = 0; j < gBh2.size(); ++j) gBh2[j] += delta_out[j];
    }

    const FP invB = FP(1) / FP(B);

#pragma omp parallel for
    for (size_t i = 0; i < win.size(); ++i)
      for (size_t j = 0; j < win[0].size(); ++j) win[i][j] -= eta * gWin[i][j] * invB;

#pragma omp parallel for
    for (size_t j = 0; j < bin.size(); ++j) bin[j] -= eta * gBin[j] * invB;

#pragma omp parallel for
    for (size_t i = 0; i < wh1.size(); ++i)
      for (size_t j = 0; j < wh1[0].size(); ++j) wh1[i][j] -= eta * gWh1[i][j] * invB;

#pragma omp parallel for
    for (size_t j = 0; j < bh1.size(); ++j) bh1[j] -= eta * gBh1[j] * invB;

#pragma omp parallel for
    for (size_t i = 0; i < wh2.size(); ++i)
      for (size_t j = 0; j < wh2[0].size(); ++j) wh2[i][j] -= eta * gWh2[i][j] * invB;

#pragma omp parallel for
    for (size_t j = 0; j < bh2.size(); ++j) bh2[j] -= eta * gBh2[j] * invB;

    loss_ = loss_sum * invB;
  }

 public:
  FFN(size_t (&ls)[4], ACTIVATION_TYPE (&acts)[3], LOSS_TYPE lt = LOSS_TYPE::MSE) : loss_t(lt) {
    std::copy(acts, acts + 2, act_s);
    gen.seed(dev.entropy());

    dis.param({0, std::sqrt(ls[0])});
    win.resize(ls[0], std::vector<FP>(ls[1]));
#pragma omp parallel for
    for (size_t i = 0; i < win.size(); ++i)
      for (size_t j = 0; j < win[0].size(); ++j) win[i][j] = dis(gen);
    bin.assign(ls[1], epsilon);

    dis.param({0, std::sqrt(ls[1])});
    wh1.resize(ls[1], std::vector<FP>(ls[2]));
#pragma omp parallel for
    for (size_t i = 0; i < wh1.size(); ++i)
      for (size_t j = 0; j < wh1[0].size(); ++j) wh1[i][j] = dis(gen);
    bh1.assign(ls[2], epsilon);

    dis.param({0, std::sqrt(ls[2])});
    wh2.resize(ls[2], std::vector<FP>(ls[3]));
#pragma omp parallel for
    for (size_t i = 0; i < wh2.size(); ++i)
      for (size_t j = 0; j < wh2[0].size(); ++j) wh2[i][j] = dis(gen);
    bh2.assign(ls[3], epsilon);
  }

  FP   get_loss() const { return loss_; }
  void set_eta(FP lr) { eta = lr; }
  void set_epsilon(FP eps) { epsilon = eps; }
  void set_loss_type(LOSS_TYPE lt) { loss_t = lt; }

  void            train_batch(const std::vector<std::vector<FP>>& X, const std::vector<std::vector<FP>>& Y) { backward(X, Y); }
  std::vector<FP> predict(const std::vector<FP>& x) { return forward(x); }
};

}  // namespace NN

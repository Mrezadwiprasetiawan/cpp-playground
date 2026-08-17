#include <array>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <matrix.hxx>
#include <random>
#include <vec.hxx>
#include <vector>

template <std::floating_point T, int N, int LAYERS>
class NeuralNetwork {
 private:
  std::array<Linear::Mat<T, N>, LAYERS> weights;
  std::array<Linear::Vec<T, N>, LAYERS> biases;

 public:
  struct Sample {
    Linear::Vec<T, N> input;
    Linear::Vec<T, N> target;
  };

  NeuralNetwork() {
    std::random_device rd;
    std::mt19937       gen(rd());

    const T                     stddev = T(1) / std::sqrt(T(N));
    std::normal_distribution<T> dist(T(0), stddev);

    for (int layer = 0; layer < LAYERS; ++layer) {
      for (int i = 0; i < N; ++i) {
        biases[layer][i] = T(0);

        for (int j = 0; j < N; ++j) weights[layer].set_element(i * N + j, dist(gen));
      }
    }
  }

  Linear::Mat<T, N> &weight(int layer) { return weights[layer]; }

  const Linear::Mat<T, N> &weight(int layer) const { return weights[layer]; }

  Linear::Vec<T, N> &bias(int layer) { return biases[layer]; }

  const Linear::Vec<T, N> &bias(int layer) const { return biases[layer]; }

  Linear::Vec<T, N> forward(Linear::Vec<T, N> input) const {
    for (int layer = 0; layer < LAYERS; ++layer) {
      input = weights[layer] * input + biases[layer];

      if (layer != LAYERS - 1) {
        for (int i = 0; i < N; ++i) input[i] = activation(input[i]);
      }
    }

    return input;
  }

  // bikin batch data acak (Gaussian standar) buat input maupun target,
  // cuma buat uji pipeline training, bukan data yang punya arti apa-apa
  static std::vector<Sample> generate_random_batch(std::size_t batch_size, std::mt19937 &gen) {
    std::normal_distribution<T> dist(T(0), T(1));
    std::vector<Sample>         batch;
    batch.reserve(batch_size);

    for (std::size_t s = 0; s < batch_size; ++s) {
      Linear::Vec<T, N> in, tgt;
      for (int i = 0; i < N; ++i) in[i] = dist(gen);
      for (int i = 0; i < N; ++i) tgt[i] = dist(gen);
      batch.push_back({in, tgt});
    }

    return batch;
  }

  // mini-batch gradient descent dengan MSE loss.
  // layer terakhir dianggap linear (sama seperti forward()), jadi dL/dz = dL/da di situ.
  // return: rata-rata loss buat batch ini
  T train_batch(const std::vector<Sample> &batch, T learning_rate) {
    // akumulator gradient disimpan sebagai array polos (bukan Mat/Vec)
    // karena Mat/Vec sengaja ga nyediain read-modify-write per elemen (ga ada operator())
    std::array<std::array<T, N * N>, LAYERS> grad_w{};
    std::array<std::array<T, N>, LAYERS>     grad_b{};
    T                                        total_loss = 0;

    for (const auto &sample : batch) {
      // forward pass, simpan aktivasi tiap layer (activations[0] = input asli)
      // dan pre-activation z (dipakai buat turunan sigmoid saat backprop)
      std::array<Linear::Vec<T, N>, LAYERS + 1> activations;
      std::array<Linear::Vec<T, N>, LAYERS>     zs;

      activations[0] = sample.input;
      for (int layer = 0; layer < LAYERS; ++layer) {
        Linear::Vec<T, N> z = weights[layer] * activations[layer] + biases[layer];
        zs[layer]           = z;

        if (layer != LAYERS - 1) {
          for (int i = 0; i < N; ++i) z[i] = activation(z[i]);
        }
        activations[layer + 1] = z;
      }

      // MSE loss + delta di layer terakhir
      const Linear::Vec<T, N> &output = activations[LAYERS];
      Linear::Vec<T, N>        delta;
      for (int i = 0; i < N; ++i) {
        T diff      = output[i] - sample.target[i];
        total_loss += diff * diff / T(N);
        delta[i]    = T(2) * diff / T(N);
      }

      // backprop dari layer terakhir mundur ke layer pertama
      for (int layer = LAYERS - 1; layer >= 0; --layer) {
        for (int i = 0; i < N; ++i) {
          grad_b[layer][i] += delta[i];
          for (int j = 0; j < N; ++j) grad_w[layer][i * N + j] += delta[i] * activations[layer][j];
        }

        if (layer == 0) break;  // ga ada layer sebelum input, stop di sini

        // propagate delta ke layer sebelumnya: W^T * delta, dikali turunan sigmoid
        // di titik z layer sebelumnya (semua layer sebelum yang terakhir pakai sigmoid)
        Linear::Vec<T, N> prev_delta;
        for (int j = 0; j < N; ++j) {
          T sum = 0;
          for (int i = 0; i < N; ++i) sum += weights[layer][i][j] * delta[i];
          prev_delta[j] = sum * activation_derivative(zs[layer - 1][j]);
        }
        delta = prev_delta;
      }
    }

    // rata-ratakan gradient di seluruh batch, lalu update weights & biases
    const T scale = learning_rate / static_cast<T>(batch.size());
    for (int layer = 0; layer < LAYERS; ++layer) {
      Linear::Mat<T, N> grad_mat;
      for (int idx = 0; idx < N * N; ++idx) grad_mat.set_element(idx, grad_w[layer][idx]);

      weights[layer] -= grad_mat * scale;

      for (int i = 0; i < N; ++i) biases[layer][i] -= scale * grad_b[layer][i];
    }

    return total_loss / static_cast<T>(batch.size());
  }

 private:
  static T activation(T x) { return T(1) / (T(1) + std::exp(-x)); }

  // turunan sigmoid dihitung dari pre-activation z: sigmoid(z) * (1 - sigmoid(z))
  static T activation_derivative(T z) {
    T s = activation(z);
    return s * (1 - s);
  }
};

int main() {
  using namespace Linear;

  // ukuran matrix/vector cuma didefinisikan di sini; NeuralNetwork, input,
  // dan loop output semuanya ngikutin nilai ini secara otomatis
  constexpr int N      = 128;
  constexpr int LAYERS = 3;

  std::random_device rd;
  std::mt19937       gen(rd());

  NeuralNetwork<double, N, LAYERS> nn;

  const std::size_t batch_size = 32;
  const double      lr         = 0.05;
  const int         epochs     = 500;

  for (int epoch = 0; epoch < epochs; ++epoch) {
    auto   batch = nn.generate_random_batch(batch_size, gen);
    double loss  = nn.train_batch(batch, lr);

    if (epoch % 50 == 0) std::cout << "epoch " << epoch << " loss = " << loss << '\n';
  }

  // isi input lewat loop, bukan initializer-list literal, biar jumlah elemennya
  // otomatis ngikutin N berapapun tanpa perlu diedit manual
  Vec<double, N> input;
  for (int i = 0; i < N; ++i) input[i] = static_cast<double>(i + 1);

  Vec<double, N> output = nn.forward(input);

  for (int i = 0; i < N; ++i) std::cout << output[i] << '\n';
  std::cout << '\n';
}
/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan

  This project contains various experiments and explorations in C++,
  including topics such as number systems, neural networks, and 
  visualizations of prime number patterns.

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


#pragma once

#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <vector>

namespace elc {
template <typename T>
struct node {
  size_t n;
  std::vector<std::shared_ptr<T>> data, muatan, impedansi;
  double timestep;

  node(size_t n, double timestep = 1e-7)
      : n(n), data(n), muatan(n), impedansi(n), timestep(timestep) {
    for (size_t i = 0; i < n; ++i) {
      data[i] = std::make_shared<T>(0);
      muatan[i] = std::make_shared<T>(0);
      impedansi[i] = std::make_shared<T>(1);
    }
  }

  virtual ~node() = default;

  T& operator[](size_t i) { return this->data[i]; }
  const T& operator[](size_t i) const { return this->data[i]; }
  T& q(size_t i) { return *muatan[i]; }
  const T& q(size_t i) const { return *muatan[i]; }

  virtual void update_tegangan() {
    for (size_t i = 0; i < n; ++i)
      this->data[i] = (*muatan[i]) * (*impedansi[i]);
  }

  void connect(node<T>& other, size_t pin_this, size_t pin_other) {
    if (pin_this >= n || pin_other >= other.n) return;
    data[pin_this] = other.data[pin_other];
    muatan[pin_this] = other.muatan[pin_other];
    impedansi[pin_this] = other.impedansi[pin_other];
  }

  void set_timestep(double t) { timestep = t; }

  void run_loop(size_t iterations = 100) {
    for (size_t i = 0; i < iterations; ++i) {
      auto start = std::chrono::high_resolution_clock::now();
      update_tegangan();
      auto end = std::chrono::high_resolution_clock::now();
      double dur = std::chrono::duration<double>(end - start).count();
      double sleep_time = timestep - dur;
      if (sleep_time > 0)
        std::this_thread::sleep_for(std::chrono::duration<double>(sleep_time));
    }
  }
};

template <typename T>
struct kapasitor : node<T> {
  T c;
  double dt;
  T i;
  kapasitor(T c, double dt, double ts = 1e-7)
      : node<T>(2, ts), c(c), dt(dt), i(0) {}

  void alirkan_arus(T iin) {
    i = iin;
    this->q(0) += i * dt;
    this->q(1) -= i * dt;
  }

  void update_tegangan() override {
    T v = (this->q(0) - this->q(1)) / c;
    this->data[0] = v;
    this->data[1] = -v;
  }
};

template <typename T>
struct induktor : node<T> {
  T l;
  double dt;
  T i;
  induktor(T l, double dt, double ts = 1e-7)
      : node<T>(2, ts), l(l), dt(dt), i(0) {}

  void terapkan_tegangan(T v) { i += (v / l) * dt; }

  void update_tegangan() override {
    this->data[0] = i;
    this->data[1] = -i;
  }
};

template <typename T>
struct resistor : node<T> {
  T r;
  resistor(T r, double ts = 1e-7) : node<T>(2, ts), r(r) {}

  void update_tegangan() override {
    T v = this->q(0) * r;
    this->data[0] = v;
    this->data[1] = 0;
  }
};

template <typename T>
struct transistor : node<T> {
  T beta, vth;
  transistor(T beta, T vth, double ts = 1e-7)
      : node<T>(3, ts), beta(beta), vth(vth) {}

  void update_tegangan() override {
    T vbe = (this->data[1] - this->data[2]);
    T ic = vbe > vth ? beta * (vbe - vth) : 0;
    this->q(0) = ic;
    this->q(2) = -ic;
    this->data[0] = ic;
    this->data[1] = vbe;
    this->data[2] = 0;
  }
};

template <typename T>
struct memristor : node<T> {
  T m;
  memristor(T m, double ts = 1e-7) : node<T>(2, ts), m(m) {}

  void update_tegangan() override {
    T v = m * this->q(0);
    this->data[0] = v;
    this->data[1] = -v;
  }
};

template <typename T>
struct dioda : node<T> {
  T i0, vt;
  dioda(T i0, T vt, double ts = 1e-7) : node<T>(2, ts), i0(i0), vt(vt) {}

  void update_tegangan() override {
    T iin = this->q(0);
    T v = iin > 0 ? vt * std::log(iin / i0) : 0;
    this->data[0] = v;
    this->data[1] = 0;
  }
};

template <typename T>
struct sumber_tegangan : node<T> {
  T vsrc;
  sumber_tegangan(T vsrc, double ts = 1e-7) : node<T>(2, ts), vsrc(vsrc) {}

  void update_tegangan() override {
    this->data[0] = vsrc;
    this->data[1] = 0;
  }
};
}  // namespace elc

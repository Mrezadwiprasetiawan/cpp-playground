#pragma once

#include <memory>
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

namespace ELC{
template<typename T>
struct Node {
  size_t n;
  std::vector<std::shared_ptr<T>> data, muatan, impedansi;
  double timestep;

  Node(size_t n, double timestep = 1e-7)
      : n(n), data(n), muatan(n), impedansi(n), timestep(timestep) {
    for (size_t i = 0; i < n; ++i) {
      data[i]      = std::make_shared<T>(0);
      muatan[i]    = std::make_shared<T>(0);
      impedansi[i] = std::make_shared<T>(1);
    }
  }

  virtual ~Node() = default;

  T& operator[](size_t i)             { return this->data[i]; }
  const T& operator[](size_t i) const { return this->data[i]; }
  T& q(size_t i)                      { return *muatan[i]; }
  const T& q(size_t i) const          { return *muatan[i]; }

  virtual void updateTegangan() {
    for (size_t i = 0; i < n; ++i)
      this->data[i] = (*muatan[i]) * (*impedansi[i]);
  }

  void connect(Node<T>& other, size_t pinThis, size_t pinOther) {
    if (pinThis >= n || pinOther >= other.n) return;
    data[pinThis]      = other.data[pinOther];
    muatan[pinThis]    = other.muatan[pinOther];
    impedansi[pinThis] = other.impedansi[pinOther];
  }

  void setTimestep(double t) { timestep = t; }

  void runLoop(size_t iterations = 100) {
    for (size_t i = 0; i < iterations; ++i) {
      auto start = std::chrono::high_resolution_clock::now();
      updateTegangan();
      auto end   = std::chrono::high_resolution_clock::now();
      double dur = std::chrono::duration<double>(end - start).count();
      double sleepTime = timestep - dur;
      if (sleepTime > 0)
        std::this_thread::sleep_for(std::chrono::duration<double>(sleepTime));
    }
  }
};

template<typename T>
struct Kapasitor : Node<T> {
  T C; double dt; T I;
  Kapasitor(T C, double dt, double ts = 1e-7)
      : Node<T>(2, ts), C(C), dt(dt), I(0) {}

  void alirkanArus(T Iin) {
    I         = Iin;
    this->q(0)     += I * dt;
    this->q(1)     -= I * dt;
  }

  void updateTegangan() override {
    T V = (this->q(0) - this->q(1)) / C;
    this->data[0] = V;
    this->data[1] = -V;
  }
};

template<typename T>
struct Induktor : Node<T> {
  T L; double dt; T I;
  Induktor(T L, double dt, double ts = 1e-7)
      : Node<T>(2, ts), L(L), dt(dt), I(0) {}

  void terapkanTegangan(T V) {
    I += (V / L) * dt;
  }

  void updateTegangan() override {
    this->data[0] = I;
    this->data[1] = -I;
  }
};

template<typename T>
struct Resistor : Node<T> {
  T R;
  Resistor(T R, double ts = 1e-7)
      : Node<T>(2, ts), R(R) {}

  void updateTegangan() override {
    T V = this->q(0) * R;
    this->data[0] = V;
    this->data[1] = 0;
  }
};

template<typename T>
struct Transistor : Node<T> {
  T beta, Vth;
  Transistor(T beta, T Vth, double ts = 1e-7)
      : Node<T>(3, ts), beta(beta), Vth(Vth) {}

  void updateTegangan() override {
    T Vbe = (this->data[1] - this->data[2]);
    T Ic  = Vbe > Vth ? beta * (Vbe - Vth) : 0;
    this->q(0) = Ic;   // kolektor muatan
    this->q(2) = -Ic;  // emitor muatan
    this->data[0] = Ic;
    this->data[1] = Vbe;
    this->data[2] = 0;
  }
};

template<typename T>
struct Memristor : Node<T> {
  T M;
  Memristor(T M, double ts = 1e-7)
      : Node<T>(2, ts), M(M) {}

  void updateTegangan() override {
    T V = M * this->q(0);
    this->data[0] = V;
    this->data[1] = -V;
  }
};

template<typename T>
struct Dioda : Node<T> {
  T I0, VT;
  Dioda(T I0, T VT, double ts = 1e-7)
      : Node<T>(2, ts), I0(I0), VT(VT) {}

  void updateTegangan() override {
    T Iin = this->q(0);
    T V   = Iin > 0 ? VT * std::log(Iin / I0) : 0;
    this->data[0] = V;
    this->data[1] = 0;
  }
};

template<typename T>
struct SumberTegangan : Node<T> {
  T Vsrc;
  SumberTegangan(T Vsrc, double ts = 1e-7)
      : Node<T>(2, ts), Vsrc(Vsrc) {}

  void updateTegangan() override {
    this->data[0] = Vsrc;
    this->data[1] = 0;
  }
};
}

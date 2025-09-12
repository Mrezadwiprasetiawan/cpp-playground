#include <collatz_path.hxx>
#include <concepts>
#include <cstdint>
#include <integer_pow.hxx>
#include <iostream>
#include <string>

#include "collatz_cube.hxx"

void print_help() {
  using namespace std;
  cout << "-s or --seed to set the seed" << endl;
  cout << "-e or --exp to activate expMode" << endl << "\twhen the next argument passed as integer, it will be setted as 2^seed*odd -1";
}

template <std::integral I>
struct v_collatz {
  I v2, v3, o;
};

template <std::integral I>
v_collatz<I> get_valuation(I value) {
  I v2 = 0, v3 = 0, o;
  ++value;
  while (!(value & 1)) {
    value >>= 1;
    ++v2;
  }
  while (!(value % 3)) {
    value /= 3;
    ++v3;
  }
  o = value;
  return {v2, v3, o};
}

template <std::integral I>
std::string superscript(I val) {
  static const char* sup_digits[] = {"⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"};
  if (val == 0) return "⁰";
  std::string res;
  char        buf[64];
  int         idx = 0;
  while (val > 0) {
    int digit   = val % 10;
    buf[idx++]  = digit;
    val        /= 10;
  }
  for (int i = idx - 1; i >= 0; --i) { res += sup_digits[buf[i]]; }
  return res;
}

int main(int argc, const char** argv) {
  using namespace std;
  uint64_t seed = 0, odd = 1;
  bool     expMode = false, outputExpMode = false, debug = false;
  for (int i = 1; i < argc; ++i) {
    if ((string(argv[i]) == "-s" || string(argv[i]) == "--seed") && argc > i + 1) seed = stoull(argv[i + 1]);
    if ((string(argv[i]) == "-e" || string(argv[i]) == "--exp")) {
      expMode = true;
      if (argc > i + 1 && argv[i + 1][0] != '-') odd = stoull(string(argv[i + 1]));
    }
    if (string(argv[i]) == "-o" || string(argv[i]) == "--outputExp") outputExpMode = true;
    if (string(argv[i]) == "-d" || string(argv[i]) == "-debug") debug = false;
  }
  if (seed == 0) {
    print_help();
    return -1;
  }
  vector<uint64_t> result;
  if (expMode) seed = int_pow<uint64_t>(2, seed) * odd - 1;
  result = collatz_get_path(seed);
  cout << "path for ";
  if (outputExpMode) {
    v_collatz vSeed = get_valuation(seed);
    cout << "2" << superscript(vSeed.v2) << " * " << "3" << superscript(vSeed.v3) << " * " << vSeed.o << " - 1 " << " = ";
    for (auto val : result) {
      v_collatz vVal = get_valuation(val);
      cout << "2" << superscript(vVal.v2) << " * " << "3" << superscript(vVal.v3) << " * " << vVal.o << " - 1 => ";
    }
  } else {
    cout << seed << " = ";
    for (auto val : result) cout << " " << val;
  }
  Collatz_cube<uint64_t> testCube;
  if (debug)
    for (size_t i = 0; i < 100; ++i)
      for (size_t j = 0; j < 100; ++j)
        for (size_t k = 0; k < 100; ++k) cout << testCube[i][j][k] << endl;
  cout << endl;
  return 0;
}

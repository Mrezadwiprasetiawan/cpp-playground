#include <collatz_path.hxx>
#include <concepts>
#include <cstdint>
#include <iostream>
#include <string>

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
  switch (val) {
    case 0: return "⁰";
    case 1: return "¹";
    case 2: return "²";
    case 3: return "³";
    case 4: return "⁴";
    case 5: return "⁵";
    case 6: return "⁶";
    case 7: return "⁷";
    case 8: return "⁸";
    case 9: return "⁹";
    default: return "^" + std::to_string(val);  // fallback
  }
}

int main(int argc, const char **argv) {
  using namespace std;
  if (argc < 3) print_help();
  uint64_t seed = 0, odd = 1;
  bool     expMode = false, outputExpMode = false;
  for (int i = 1; i < argc; ++i) {
    if ((string(argv[i]) == "-s" || string(argv[i]) == "--seed") && argc > i + 1) seed = stoull(argv[i + 1]);
    if ((string(argv[i]) == "-e" || string(argv[i]) == "--exp")) {
      expMode = true;
      if (argc > i + 1 && argv[i + 1][0] != '-') odd = stoull(string(argv[i + 1]));
    }
    if ((string(argv[i]) == "-o" || string(argv[i]) == "--outputExp")) outputExpMode = true;
  }
  if (seed == 0) {
    print_help();
    return -1;
  }
  vector<uint64_t> result;
  if (expMode) {
    seed   = int_pow<uint64_t>(2, seed) * odd - 1;
    result = collatz_get_path(seed);
  } else result = collatz_get_path(seed);
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
  cout << endl;
  return 0;
}
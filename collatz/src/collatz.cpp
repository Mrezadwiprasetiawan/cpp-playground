#include <collatz_path.hxx>
#include <cstdint>
#include <iostream>
#include <string>

void print_help() {
  using namespace std;
  cout << "-s or --seed to set the seed" << endl;
  cout << "-e or --exp to activate expMode" << endl << "\twhen the next argument passed as integer, it will be setted as 2^seed*odd -1";
}

int main(int argc, const char **argv) {
  using namespace std;
  if (argc < 3) print_help();
  uint64_t seed = 0, odd = 1;
  bool     expMode = false;
  for (int i = 1; i < argc; ++i) {
    if ((string(argv[i]) == "-s" || string(argv[i]) == "--seed") && argc > i + 1) seed = stoull(argv[i + 1]);
    if ((string(argv[i]) == "-e" || string(argv[i]) == "--exp")) {
      expMode = true;
      if (argc > i + 1) odd = stoull(string(argv[i + 1]));
    }
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
  cout << "path for " << seed << " =";
  for (auto val : result) cout << " " << val;
  cout << endl;
  return 0;
}
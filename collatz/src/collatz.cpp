#include <collatz_cube.hxx>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

using U64 = uint64_t;

static void print_help(const char *prog) {
  std::cerr <<
    "Usage:\n"
    "  " << prog << " tensor [Z] [Y] [X]      Print tensor values (default Z=Y=X=5)\n"
    "  " << prog << " path   [FROM] [TO]       Print Collatz paths for seeds FROM..TO (default 1..5)\n"
    "  " << prog << " all    [Z] [Y] [X]       Both tensor and paths, seeds 1..Z\n"
    "  " << prog << " -h | --help              Show this help\n"
    "\n"
    "Options:\n"
    "  Z, Y, X     Tensor dimensions (uint64, Z=depth, Y=row, X=col)\n"
    "  FROM, TO    Seed range inclusive (uint64, FROM >= 1)\n"
    "\n"
    "Examples:\n"
    "  " << prog << " tensor 3 4 5\n"
    "  " << prog << " path 1 20\n"
    "  " << prog << " all 6\n";
}

static void do_tensor(U64 Z, U64 Y, U64 X) {
  for (U64 z = 0; z < Z; ++z) {
    for (U64 y = 0; y < Y; ++y) {
      for (U64 x = 0; x < X; ++x) {
        std::cout << CollatzCube<U64, U64>::get_value_from_index(z, y, x);
        if (x + 1 < X) std::cout << '\t';
      }
      std::cout << '\n';
    }
    std::cout << '\n';
  }
}

static void do_path(U64 from, U64 to) {
  for (U64 n = from; n <= to; ++n) {
    std::cout << "seed = " << n << '\n';
    for (auto i : CollatzCube<U64, U64>::get_path_index_from_seed(n))
      std::cout << i.z << ',' << i.y << ',' << i.x << '\n';
    std::cout << '\n';
  }
}

int main(int argc, const char **argv) {
  using namespace std;

  if (argc < 2 || string_view(argv[1]) == "-h" || string_view(argv[1]) == "--help") {
    print_help(argv[0]);
    return argc < 2 ? 1 : 0;
  }

  string_view mode(argv[1]);

  if (mode == "tensor") {
    U64 Z = argc > 2 ? stoull(argv[2]) : 5;
    U64 Y = argc > 3 ? stoull(argv[3]) : 5;
    U64 X = argc > 4 ? stoull(argv[4]) : 5;
    do_tensor(Z, Y, X);

  } else if (mode == "path") {
    U64 from = argc > 2 ? stoull(argv[2]) : 1;
    U64 to   = argc > 3 ? stoull(argv[3]) : 5;
    if (from < 1) { cerr << "Error: FROM must be >= 1\n"; return 1; }
    if (from > to) { cerr << "Error: FROM must be <= TO\n"; return 1; }
    do_path(from, to);

  } else if (mode == "all") {
    U64 Z = argc > 2 ? stoull(argv[2]) : 5;
    U64 Y = argc > 3 ? stoull(argv[3]) : 5;
    U64 X = argc > 4 ? stoull(argv[4]) : 5;
    do_tensor(Z, Y, X);
    do_path(1, Z);

  } else {
    cerr << "Error: unknown mode '" << mode << "'\n\n";
    print_help(argv[0]);
    return 1;
  }

  return 0;
}

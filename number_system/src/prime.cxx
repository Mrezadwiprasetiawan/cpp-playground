#include "numbers.hxx"
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

#define USING_OSTREAM                                                          \
  using std::cout;                                                             \
  using std::endl;

std::string argname[] = {"-h", "-help", "-l", "-limit", "-s", "-size"};

uint64_t to_number(char *number) {
  using std::runtime_error;
  uint64_t result = 0;
  size_t len = strlen(number);
  for (size_t i = 0; i < len; ++i) {
    char c = number[i];
    if (!std::isdigit(c)) {
      throw runtime_error("Invalid character '" + std::string(1, c) +
                          "' in number.");
    }
    result = result * 10 + (c - '0');
  }
  return result;
}

void printHelp() {
  USING_OSTREAM;
  cout << "Print prime number sequences" << endl;
  cout << "\t-h -help\tprint this help" << endl;
  cout << "\t-l -limit <number>\tprint prime numbers up to limit using sieve "
          "of eratosthenes"
       << endl;
  cout << "\t-s -size <number>\tprint first n prime numbers using optimized "
          "trial division"
       << endl;
}

void do_l(uint64_t limit) {
  USING_OSTREAM;
  Prime<uint64_t> prime;
  for (uint64_t prime : prime.from_range_limit(limit))
    cout << prime << endl;
}

void do_s(size_t size) {
  USING_OSTREAM;
  Prime<uint64_t> prime;
  for (uint64_t prime : prime.from_size(size))
    cout << prime << endl;
}

int main(int argc, char *argv[]) {
  using std::string;
  if (argc == 1) {
    printHelp();
    return 0;
  }

  int arg_l = -1, arg_s = -1;
  for (int i = 1; i < argc; i++) {
    string arg = argv[i];
    if (arg == argname[0] || arg == argname[1]) {
      printHelp();
      return 0;
    } else if (arg == argname[2] || arg == argname[3]) {
      if (arg_l != -1) {
        std::cerr << "Error: Multiple -l options specified" << std::endl;
        return 1;
      }
      arg_l = i;
    } else if (arg == argname[4] || arg == argname[5]) {
      if (arg_s != -1) {
        std::cerr << "Error: Multiple -s options specified" << std::endl;
        return 1;
      }
      arg_s = i;
    }
  }

  if (arg_l != -1 && arg_s != -1) {
    std::cerr << "Error: Cannot specify both -l and -s options" << std::endl;
    printHelp();
    return 1;
  }

  try {
    if (arg_l != -1) {
      if (arg_l + 1 >= argc) {
        std::cerr << "Error: Missing argument for -l option" << std::endl;
        return 1;
      }
      do_l(to_number(argv[arg_l + 1]));
    } else if (arg_s != -1) {
      if (arg_s + 1 >= argc) {
        std::cerr << "Error: Missing argument for -s option" << std::endl;
        return 1;
      }
      do_s(to_number(argv[arg_s + 1]));
    } else {
      printHelp();
      return 1;
    }
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}

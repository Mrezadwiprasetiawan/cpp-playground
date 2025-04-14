#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>

#include "numbers.hxx"

#define USING_OSTREAM \
  using std::cout;    \
  using std::endl;

std::string argname[] = {"-h", "-help", "-l", "-limit", "-s", "-size"};

uint64_t to_number_with_suffix(const char *str) {
  size_t len = strlen(str);
  uint64_t num = 0;
  size_t i = 0;

  while (i < len && std::isdigit(str[i])) {
    num = num * 10 + (str[i++] - '0');
  }

  if (i == len) return num;

  char suffix = std::tolower(str[i]);
  switch (suffix) {
    case 'k':
      return num << 10;
    case 'm':
      return num << 20;
    case 'g':
      return num << 30;
    case 't':
      return num << 40;
    case 'p':
      return num << 50;
    case 'e':
      return num << 60;
    default:
      std::cerr << "Invalid suffix: " << str[i] << std::endl;
      std::exit(1);
  }
}

void printHelp() {
  USING_OSTREAM;
  cout << "Print prime number sequences" << endl;
  cout << "\t-h -help\tprint this help" << endl;
  cout << "\t-l -limit <number>\tprint primes up to limit (supports K/M/G)"
       << endl;
  cout << "\t-s -size <number>\tprint first N primes (supports K/M/G)" << endl;
}

void do_l(uint64_t limit) {
  USING_OSTREAM;
  Prime<uint64_t> prime;
  for (uint64_t p : prime.from_range_limit(limit)) cout << p << endl;
}

void do_s(size_t size) {
  USING_OSTREAM;
  Prime<uint64_t> prime;
  for (uint64_t p : prime.from_size(size)) cout << p << endl;
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
    return 1;
  }

  if (arg_l != -1) {
    if (arg_l + 1 >= argc) {
      std::cerr << "Error: Missing argument for -l option" << std::endl;
      return 1;
    }
    do_l(to_number_with_suffix(argv[arg_l + 1]));
  } else if (arg_s != -1) {
    if (arg_s + 1 >= argc) {
      std::cerr << "Error: Missing argument for -s option" << std::endl;
      return 1;
    }
    do_s(to_number_with_suffix(argv[arg_s + 1]));
  } else {
    printHelp();
    return 1;
  }

  return 0;
}

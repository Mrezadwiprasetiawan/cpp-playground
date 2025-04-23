#include <cctype>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "numbers.hxx"

#define USING_OSTREAM \
  using std::cout;    \
  using std::endl;

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
  cout << "\t-h -help\t\tprint this help" << endl;
  cout << "\t-l -limit <number>\tprint primes up to limit (supports K/M/G)"
       << endl;
  cout << "\t-s -size <number>\tprint first N primes (supports K/M/G)" << endl;
  cout << "\t-n -isprime <number>\tcheck if number is prime" << endl;
  cout << "\t-i -index <number>\tprint the i-th prime (0-based)" << endl;
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
void do_n(uint64_t value) {
  USING_OSTREAM;
  Prime<uint64_t> prime;
  if (prime.is_prime(value)) {
    cout << value << " is prime" << endl;
    return;
  }
  cout << value << " is not prime" << endl;
}

void do_i(size_t index) {
  USING_OSTREAM;
  Prime<uint64_t> prime;
  std::vector<uint64_t> primes = prime.from_size(index + 1);
  if (index < primes.size()) {
    cout << "Prime #" << index << ": " << primes[index] << endl;
  } else {
    std::cerr << "Index out of bounds" << endl;
  }
}

int main(int argc, char *argv[]) {
  using std::string;
  using std::vector;

  if (argc == 1) {
    printHelp();
    return 0;
  }

  std::vector<string> main_args = {"-h", "-l", "-s", "-n", "-i"};
  std::vector<string> alter_args = {"-help", "-limit", "-size", "-isprime",
                                    "-index"};

  int found_index = -1;
  int arg_pos = -1;

  for (int i = 1; i < argc; i++) {
    string arg = argv[i];
    for (size_t j = 0; j < main_args.size(); j++) {
      if (arg == main_args[j] || arg == alter_args[j]) {
        if (found_index != -1) {
          std::cerr << "Error: Multiple options specified" << std::endl;
          return 1;
        }
        found_index = j;
        arg_pos = i;
      }
    }
  }

  if (found_index == -1) {
    printHelp();
    return 1;
  }

  switch (found_index) {
    case 0:  // -h
      printHelp();
      return 0;
    case 1:  // -l
      if (arg_pos + 1 >= argc) {
        std::cerr << "Error: Missing argument for -l option" << std::endl;
        return 1;
      }
      do_l(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 2:  // -s
      if (arg_pos + 1 >= argc) {
        std::cerr << "Error: Missing argument for -s option" << std::endl;
        return 1;
      }
      do_s(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 3:  // -n
      if (arg_pos + 1 >= argc) {
        std::cerr << "Error: Missing argument for -n option" << std::endl;
        return 1;
      }
      do_n(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 4:  // -i
      if (arg_pos + 1 >= argc) {
        std::cerr << "Error: Missing argument for -i option" << std::endl;
        return 1;
      }
      do_i(to_number_with_suffix(argv[arg_pos + 1]));
      break;
  }

  return 0;
}

/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan


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


#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "numbers.hxx"

uint64_t to_number_with_suffix(const char *str) {
  using namespace std;
  size_t len = strlen(str);
  uint64_t num = 0;

  for (int i = 0; i < len; ++i) {
    char c = str[i];
    if (c >= '0' && c <= '9') num = num * 10 + (c - '0');
    else {
      char suffix = tolower(str[i]);
      switch (suffix) {
        case 'k': return num << 10;
        case 'm': return num << 20;
        case 'g': return num << 30;
        case 't': return num << 40;
        case 'p': return num << 50;
        case 'e': return num << 60;
        default: cerr << "Invalid suffix: " << str[i] << endl; exit(1);
      }
    }
  }
  return num;
}

void printHelp() {
  using namespace std;
  cout << "Print prime number sequences" << endl;
  cout << "\t-h -help\t\tprint this help" << endl;
  cout << "\t-l -limit <number>\tprint primes up to limit (supports K/M/G)"
       << endl;
  cout << "\t-s -size <number>\tprint first N primes (supports K/M/G)" << endl;
  cout << "\t-n -isprime <number>\tcheck if number is prime" << endl;
  cout << "\t-i -index <number>\tprint the i-th prime (0-based)" << endl;
}

void do_l(uint64_t limit) {
  using namespace std;
  Prime<uint64_t> prime;
  for (uint64_t p : prime.from_range_limit(limit)) cout << p << endl;
  cout << "Prime finded using up to " << Prime<uint64_t>::max_thread() << "threads" << endl;
}

void do_s(size_t size) {
  using namespace std;
  Prime<uint64_t> prime;
  for (uint64_t p : prime.from_size(size)) cout << p << endl;
  cout << "Prime finded using up to " << Prime<uint64_t>::max_thread() << "threads" << endl;
}
void do_n(uint64_t value) {
  using namespace std;
  Prime<uint64_t> prime;
  if (prime.is_prime(value)) {
    cout << value << " is prime" << endl;
    cout << "Prime finded using up to " << Prime<uint64_t>::max_thread() << "threads" << endl;
    return;
  }
  cout << value << " is not prime" << endl;
  cout << "Prime finded using up to " << Prime<uint64_t>::max_thread() << "threads" << endl;
}

void do_i(size_t index) {
  using namespace std;
  Prime<uint64_t> prime;
  vector<uint64_t> primes = prime.from_size(index + 1);
  if (index < primes.size()) {
    cout << "Prime #" << index << ": " << primes[index] << endl;
  } else {
    cerr << "Index out of bounds" << endl;
  }
  cout << "Prime finded using " << Prime<uint64_t>::max_thread() << "threads" << endl;
}

int main(int argc, char *argv[]) {
  using namespace std;

  if (argc == 1) {
    printHelp();
    return 0;
  }

  vector<string> main_args = {"-h", "-l", "-s", "-n", "-i"};
  vector<string> alter_args = {"-help", "-limit", "-size", "-isprime",
                               "-index"};

  int found_index = -1;
  int arg_pos = -1;

  for (int i = 1; i < argc; i++) {
    string arg = argv[i];
    for (size_t j = 0; j < main_args.size(); j++) {
      if (arg == main_args[j] || arg == alter_args[j]) {
        if (found_index != -1) {
          cerr << "Error: Multiple options specified" << endl;
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
        cerr << "Error: Missing argument for -l option" << endl;
        return 1;
      }
      do_l(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 2:  // -s
      if (arg_pos + 1 >= argc) {
        cerr << "Error: Missing argument for -s option" << endl;
        return 1;
      }
      do_s(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 3:  // -n
      if (arg_pos + 1 >= argc) {
        cerr << "Error: Missing argument for -n option" << endl;
        return 1;
      }
      do_n(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 4:  // -i
      if (arg_pos + 1 >= argc) {
        cerr << "Error: Missing argument for -i option" << endl;
        return 1;
      }
      do_i(to_number_with_suffix(argv[arg_pos + 1]));
      break;
  }

  return 0;
}

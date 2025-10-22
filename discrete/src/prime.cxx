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
#include <prime.hxx>
#include <string>
#include <vector>

uint64_t to_number_with_suffix(const char *str) {
  using namespace std;
  size_t   len = strlen(str);
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
  cout << "\t-h --help\t\tprint this help" << endl;
  cout << "\t-l --limit <number>\tprint primes up to limit (supports K/M/G)" << endl;
  cout << "\t-s --size <number>\tprint first N primes (supports K/M/G)" << endl;
  cout << "\t-n --isprime <number>\tcheck if number is prime" << endl;
  cout << "\t-i --index <number>\tprint the i-th prime (0-based)" << endl;
}

void do_l(uint64_t limit) {
  using namespace std;
  using namespace Discrete;
  auto &prime = Prime<uint64_t>::instance();
  for (uint64_t p : prime.from_range_limit(limit)) cout << p << endl;
  cout << "Prime finded using up to " << Prime<uint64_t>::max_thread() << "threads" << endl;
}

void do_s(size_t size) {
  using namespace std;
  using namespace Discrete;
  auto &prime = Prime<uint64_t>::instance();
  for (uint64_t p : prime.from_size(size)) cout << p << endl;
  cout << "Prime finded using up to " << Prime<uint64_t>::max_thread() << "threads" << endl;
}
void do_n(uint64_t value) {
  using namespace std;
  using namespace Discrete;
  auto &prime = Prime<uint64_t>::instance();
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
  using namespace Discrete;
  auto            &prime  = Prime<uint64_t>::instance();
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
  using namespace Discrete;

  if (argc == 1) {
    printHelp();
    return 0;
  }

  vector<string> main_args  = {"-h", "-l", "-s", "-n", "-i"};
  vector<string> alter_args = {"--help", "--limit", "--size", "--isprime", "--index"};

  int    found_index = -1;
  int    arg_pos     = -1;
  bool   do_write    = false;
  bool   do_load     = false;
  string write_file, load_file;

  // --- deteksi opsi utama ---
  for (int i = 1; i < argc; i++) {
    string arg = argv[i];
    for (size_t j = 0; j < main_args.size(); j++) {
      if (arg == main_args[j] || arg == alter_args[j]) {
        if (found_index != -1) {
          cerr << "Error: Multiple options specified" << endl;
          return 1;
        }
        found_index = j;
        arg_pos     = i;
      }
    }
  }

  // --- deteksi opsi tambahan (write / load) ---
  for (int i = 1; i < argc - 1; i++) {
    string arg = argv[i];
    if (arg == "-w" || arg == "--write") {
      do_write   = true;
      write_file = argv[i + 1];
    } else if (arg == "-r" || arg == "--load") {
      do_load   = true;
      load_file = argv[i + 1];
    }
  }

  if (found_index == -1) {
    printHelp();
    return 1;
  }

  auto &prime = Prime<uint64_t>::instance();

  // --- load dulu kalau diminta ---
  if (do_load) {
    if (!prime.load_sieve(load_file)) {
      cerr << "Error: Failed to load sieve from file '" << load_file << "'" << endl;
      return 1;
    }
    cerr << "[load_sieve] Loaded sieve from '" << load_file << "'\n";
  }

  // --- eksekusi opsi utama ---
  switch (found_index) {
    case 0: printHelp(); return 0;
    case 1:
      if (arg_pos + 1 >= argc) {
        cerr << "Error: Missing argument for -l option" << endl;
        return 1;
      }
      do_l(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 2:
      if (arg_pos + 1 >= argc) {
        cerr << "Error: Missing argument for -s option" << endl;
        return 1;
      }
      do_s(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 3:
      if (arg_pos + 1 >= argc) {
        cerr << "Error: Missing argument for -n option" << endl;
        return 1;
      }
      do_n(to_number_with_suffix(argv[arg_pos + 1]));
      break;
    case 4:
      if (arg_pos + 1 >= argc) {
        cerr << "Error: Missing argument for -i option" << endl;
        return 1;
      }
      do_i(to_number_with_suffix(argv[arg_pos + 1]));
      break;
  }

  // --- tulis setelah selesai ---
  if (do_write) {
    if (!prime.write_sieve(write_file)) {
      cerr << "Error: Failed to write sieve to file '" << write_file << "'" << endl;
      return 1;
    }
    cerr << "[write_sieve] Saved sieve to '" << write_file << "'\n";
  }

  return 0;
}

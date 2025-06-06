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

#include <cstring>
#include <fibonacci.hxx>
#include <iostream>

std::string argname[] = {"-h", "-help", "-l", "-limit", "-i", "-index"};

uint64_t to_number(char *number) {
  using std::runtime_error;
  uint64_t result = 0;
  size_t len = strlen(number);
  for (size_t i = 0; i < len; ++i) {
    char c = number[i];
    if (!std::isdigit(c)) { throw runtime_error("Invalid character '" + std::string(1, c) + "' in number."); }
    result = result * 10 + (c - '0');
  }
  return result;
}

void printHelp() {
  using std::cout;
  using std::endl;
  cout << "Print fibonacci number sequences" << endl;
  cout << "\t-h -help\tprint this help" << endl;
  cout << "\t-l -limit <value>\tprint fibonacci numbers as much as value "
          "provided"
       << endl;
  cout << "\t-i -index <i>\tprint fibonacci numbers on index i" << endl;
}

void do_l(uint64_t limit) {
  using std::cout;
  using std::endl;
  using namespace Discrete;
  Fibonacci fbnc;
  for (std::string value : fbnc.get_all(limit)) cout << value << endl;
}

void do_i(size_t index) {
  using std::cout;
  using std::endl;
  using namespace Discrete;
  Fibonacci fbnc;
  cout << fbnc.get_index(index) << endl;
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
        std::cerr << "Error: Multiple -i options specified" << std::endl;
        return 1;
      }
      arg_s = i;
    }
  }

  if (arg_l != -1 && arg_s != -1) {
    std::cerr << "Error: Cannot specify both -l and -i options" << std::endl;
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
        std::cerr << "Error: Missing argument for -i option" << std::endl;
        return 1;
      }
      do_i(to_number(argv[arg_s + 1]));
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

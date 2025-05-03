/*
  cpp-playground - C++ experiments and learning playground
  Copyright (C) 2025 M. Reza Dwi Prasetiawan

  This project contains various experiments and explorations in C++,
  including topics such as number systems, neural networks, and 
  visualizations of prime number patterns.

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


#include <csignal>
#include <iostream>
#include <cstdlib>

void handler(int sig, siginfo_t *info, void *context) {
  std::cout << "Segfault at address: " << info->si_addr << std::endl;
  std::exit(1);
}

int main() {
  struct sigaction sa;
  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = handler;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGSEGV, &sa, nullptr);

  int *p = (int*)0xDEADBEEF; // alamat ilegal
  *p = 42;

  return 0;
}

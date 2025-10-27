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


#include <chrono>
#include <core.hxx>
#include <iostream>
#include <thread>

#define PINSIZE 8

void simulate(Board& board) {
  using namespace std::chrono;
  using namespace std::chrono_literals;

  auto start = steady_clock::now();
  while (steady_clock::now() - start < 2s) {
    board.digitalWrite(0, HIGH);
    board.analogWrite(1, 128);
  }
}

void debugger(std::vector<Pin>& pins) {
  using namespace std::chrono;
  using namespace std::chrono_literals;

  auto start = steady_clock::now();
  while (steady_clock::now() - start < 2s) {
    for (int i = 0; i < PINSIZE; ++i) std::cout << "Pin " << i << ": " << (int)pins[i].get_value() << '\n';
    std::cout << "-------------------\n";
    std::this_thread::sleep_for(10ms);
  }
}

int main() {
  Board board(PINSIZE);
  board.set_debugger(debugger);
  board.set_main(simulate);
  board.start();
  board.join();
}

#include <chrono>
#include <core.hxx>
#include <iostream>
#include <thread>

#define PINSIZE 8

void simulate(Board& board) {
  using namespace std::chrono;
  using namespace std::chrono_literals;

  auto now = steady_clock::now();
  while (steady_clock::now() - now < 10s) {
    board.digitalWrite(0, HIGH);
    board.analogWrite(1, 128);
  }
}

void debugger(std::vector<Pin>& pins) {
  using namespace std::chrono;
  using namespace std::chrono_literals;

  auto now = steady_clock::now();
  while (steady_clock::now() - now < 10s) {
    for (int i = 0; i < PINSIZE; ++i) std::cout << "Pin " << i << ": " << (int)pins[i].get_value() << '\n';
    std::cout << "-------------------\n";
    std::this_thread::sleep_for(100ms);
  }
}

int main() {
  Board board(PINSIZE);
  board.set_debugger(debugger);
  board.set_main(simulate);
  board.start();
  board.join();
}

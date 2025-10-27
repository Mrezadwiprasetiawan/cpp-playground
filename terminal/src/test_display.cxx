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


#include <array>
#include <chrono>
#include <cmath>
#include <csignal>
#include <thread>
#include <vector>

#include "display-term.hxx"

using namespace std;

static bool running = true;
void        handle_sigint(int) { running = false; }

int main() {
  signal(SIGINT, handle_sigint);
  Display& disp = Display::getInstance();
  int      w    = disp.get_width();
  int      h    = disp.get_height();

  cout << "debug : rendering on scale" << w << ',' << h << endl;
  std::this_thread::sleep_for(2s);

  vector<vector<array<int, 3>>> rgbBuffer(h, vector<array<int, 3>>(w));

  float t = 0.0f;
  while (running) {
#pragma omp parallel for
    for (int y = 0; y < h; ++y) {
      for (int x = 0; x < w; ++x) {
        float xf        = x / float(w - 1);
        float yf        = y / float(h - 1);
        int   r         = int((sin(t + xf * 3.1415f) * 0.5f + 0.5f) * 255);
        int   g         = int((sin(t + yf * 3.1415f + 2.0f) * 0.5f + 0.5f) * 255);
        int   b         = int((sin(t + xf * 3.1415f + 4.0f) * 0.5f + 0.5f) * 255);
        rgbBuffer[y][x] = {r, g, b};
      }
    }

    disp.push_buffer_bg(rgbBuffer);
    disp.render();

    t += 0.05f;
    if (t > 2 * 3.1415926f) t -= 2 * 3.1415926f;
    this_thread::sleep_for(chrono::milliseconds(16));
  }

  disp.getANSI().reset();
  disp.getANSI().showCursor();
  return 0;
};
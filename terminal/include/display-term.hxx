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

#pragma once

#include <array>
#include <atomic>
#include <cstdint>
#include <limits>
#include <mutex>
#include <thread>
#include <vector>

#include "ANSI.hxx"

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

class Display {
  using RGB = std::array<uint8_t, 3>;

 private:
  int                        width, height;
  inline static std::mutex   data_mtx;
  inline static ANSI&        ansiInstance = ANSI::getInstance();
  std::vector<std::vector<RGB>>  backgroundRGB, foregroundRGB;
  std::vector<std::vector<char>> data;
  std::thread                    runner;

#ifndef _WIN32
  inline static std::atomic<bool> resize_pending{false};

  static void sigwinch_handler(int) {
    resize_pending.store(true, std::memory_order_relaxed);
  }

  void handle_resize() {
    if (!resize_pending.load(std::memory_order_relaxed)) return;
    resize_pending.store(false, std::memory_order_relaxed);

    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != 0) return;

    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    width  = w.ws_col;
    height = w.ws_row;
    data.assign(height, vector<char>(width, ' '));
    backgroundRGB.assign(height, vector<RGB>(width, {0, 0, 0}));
    foregroundRGB.assign(height, vector<RGB>(width, {255, 255, 255}));
    /* Force full redraw setelah resize */
    ansiInstance.clearScreen();
    ansiInstance.flush();
  }
#endif

  Display() {
#ifndef _WIN32
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
      width  = w.ws_col;
      height = w.ws_row;
    } else {
      width  = 80;
      height = 24;
    }
    struct sigaction sa{};
    sa.sa_handler = sigwinch_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGWINCH, &sa, nullptr);
#else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
      width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
      height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
      width  = 80;
      height = 24;
    }
#endif
    data.resize(height, std::vector<char>(width, ' '));
    backgroundRGB.resize(height, std::vector<RGB>(width, {0, 0, 0}));
    foregroundRGB.resize(height, std::vector<RGB>(width, {255, 255, 255}));
    ansiInstance.enterAlternateScreen();
    /* Sembunyikan cursor sekali saja — tidak perlu tiap frame */
    ansiInstance.hideCursor();
    ansiInstance.flush();
  }

 public:
  Display(const Display&)            = delete;
  Display& operator=(const Display&) = delete;

  static Display& getInstance() {
    static Display instance;
    return instance;
  }

  ~Display() {
    ansiInstance.reset();
    ansiInstance.exitAlternateScreen();
    ansiInstance.showCursor();
    ansiInstance.flush();
  }

  int   get_width() const { return width; }
  int   get_height() const { return height; }
  ANSI& getANSI() { return ansiInstance; }

  std::vector<std::vector<RGB>> get_background_RGB() {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    return backgroundRGB;
  }

  std::vector<std::vector<RGB>> get_foreground_RGB() {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    return foregroundRGB;
  }

#define setter_RGB(name)                                                                                  \
  void set_##name(int r, int g, int b) {                                                                  \
    uint8_t ur = static_cast<uint8_t>(r), ug = static_cast<uint8_t>(g), ub = static_cast<uint8_t>(b);   \
    using namespace std;                                                                                   \
    lock_guard<mutex> lock(data_mtx);                                                                     \
    _Pragma("omp parallel for") for (auto& row : name##RGB) for (auto& px : row) px = {ur, ug, ub};      \
  }

  setter_RGB(background);
  setter_RGB(foreground);
#undef setter_RGB

  bool push_buffer(int row, const std::vector<char>& buffer) {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
#pragma omp parallel for
    for (size_t i = 0; i < buffer.size(); ++i) data[row][i] = buffer[i];
    return true;
  }

  bool push_buffer(int row, std::initializer_list<char> buffer) {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    size_t i = 0;
    for (auto c : buffer) data[row][i++] = c;
    return true;
  }

  bool push_buffer_bg(const std::vector<std::vector<std::array<int, 3>>>& rgb) {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    size_t rows = rgb.size();
#pragma omp parallel for
    for (size_t y = 0; y < rows; ++y)
      for (size_t x = 0; x < rgb[y].size(); ++x)
        backgroundRGB[y][x] = { static_cast<uint8_t>(rgb[y][x][0]),
                                 static_cast<uint8_t>(rgb[y][x][1]),
                                 static_cast<uint8_t>(rgb[y][x][2]) };
    return true;
  }

  std::vector<std::vector<char>>& get_frame_buffer() {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    return data;
  }

  void render() {
#ifndef _WIN32
    handle_resize();
#endif

    using namespace std;
    lock_guard<mutex> lock(data_mtx);

    const int rows = height;
    const int cols = width;

    /* Satu string per baris — tiap thread OMP tulis ke indeks miliknya sendiri,
     * tidak ada sharing sehingga tidak perlu mutex di dalam loop paralel.      */
    vector<string> row_bufs(static_cast<size_t>(rows));

    /* Estimasi kapasitas awal per baris: worst-case ~40 byte/sel */
    const size_t row_cap = static_cast<size_t>(cols) * 40;

#pragma omp parallel for schedule(static)
    for (int y = 0; y < rows; ++y) {
      string& s = row_bufs[static_cast<size_t>(y)];
      s.reserve(row_cap);

      /* Sentinel 256 = belum ada warna — paksa emit pada sel pertama */
      constexpr uint16_t NONE = 256;
      uint16_t cur_br = NONE, cur_bg_ = NONE, cur_bb = NONE;
      uint16_t cur_fr = NONE, cur_fg_ = NONE, cur_fb = NONE;

      for (int x = 0; x < cols; ++x) {
        const auto& bg = backgroundRGB[static_cast<size_t>(y)][static_cast<size_t>(x)];
        const auto& fg = foregroundRGB[static_cast<size_t>(y)][static_cast<size_t>(x)];

        if (bg[0] != cur_br || bg[1] != cur_bg_ || bg[2] != cur_bb) {
          ANSI::writeBgRGB(s, bg[0], bg[1], bg[2]);
          cur_br = bg[0]; cur_bg_ = bg[1]; cur_bb = bg[2];
        }

        if (fg[0] != cur_fr || fg[1] != cur_fg_ || fg[2] != cur_fb) {
          ANSI::writeFgRGB(s, fg[0], fg[1], fg[2]);
          cur_fr = fg[0]; cur_fg_ = fg[1]; cur_fb = fg[2];
        }

        s += data[static_cast<size_t>(y)][static_cast<size_t>(x)];
      }

      if (y != rows - 1) s += '\n';
    }

    /* Hitung total size dengan reduction — lalu gabung serial (urutan harus terjaga) */
    size_t total = 0;
#pragma omp parallel for schedule(static) reduction(+ : total)
    for (int y = 0; y < rows; ++y) total += row_bufs[static_cast<size_t>(y)].size();
    ansiInstance.reserve(total + 32); /* +32 untuk home + reset */

    ansiInstance.homeCursor();
    for (auto& s : row_bufs) ansiInstance.append(s);
    ansiInstance.reset();
    ansiInstance.flush();
  }
};

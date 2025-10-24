#pragma once

#include <array>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "ANSI.hxx"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

class Display {
 private:
  int                                          width, height;
  inline static std::mutex                     data_mtx;
  inline static ANSI&                          ansiInstance = ANSI::getInstance();
  std::vector<std::vector<std::array<int, 3>>> backgroundRGB, foregroundRGB;
  std::vector<std::vector<char>>               data;
  std::thread                                  runner;

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
    backgroundRGB.resize(height, std::vector<std::array<int, 3>>(width, {0, 0, 0}));
    foregroundRGB.resize(height, std::vector<std::array<int, 3>>(width, {255, 255, 255}));
    ansiInstance.enterAlternateScreen();
  }

 public:
  Display(const Display&)            = delete;
  Display& operator=(const Display&) = delete;

  static Display& getInstance() {
    static Display instance;
    return instance;
  }

  ~Display() {
    ansiInstance.exitAlternateScreen();
    ansiInstance.showCursor();
  }

  int   get_width() const { return width; }
  int   get_height() const { return height; }
  ANSI& getANSI() { return ansiInstance; }

  std::vector<std::vector<std::array<int, 3>>> get_background_RGB() {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    return backgroundRGB;
  }

  std::vector<std::vector<std::array<int, 3>>> get_foreground_RGB() {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    return foregroundRGB;
  }
#define setter_RGB(name)                                                                                 \
  void set_##name(int r, int g, int b) {                                                                 \
    using namespace std;                                                                                 \
    lock_guard<mutex> lock(data_mtx);                                                                    \
    _Pragma("omp parallel for") for (auto& rowrgb : name##RGB) for (auto& rgb : rowrgb) rgb = {r, g, b}; \
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
    size_t            i = 0;
#pragma omp parallel for
    for (auto c : buffer) data[row][i++] = c;
    return true;
  }

  bool push_buffer_rgb(const std::vector<std::vector<std::array<int, 3>>>& rgb) {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    size_t            rows = rgb.size();
#pragma omp parallel for
    for (size_t y = 0; y < rows; ++y)
      for (size_t x = 0; x < rgb[y].size(); ++x) backgroundRGB[y][x] = rgb[y][x];
    return true;
  }

  std::vector<std::vector<char>> get_frame_buffer() {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    return data;
  }

  void render() {
    ansiInstance.hideCursor();
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    ansiInstance.clearScreen();
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        const auto& bg = backgroundRGB[y][x];
        const auto& fg = foregroundRGB[y][x];
        ansiInstance.bgRGB(bg[0], bg[1], bg[2]);
        ansiInstance.fgRGB(fg[0], fg[1], fg[2]);
        cout << data[y][x];
      }
      if (!(y == height - 1)) cout << '\n';
    }
    cout.flush();
    ansiInstance.reset();
  }
};

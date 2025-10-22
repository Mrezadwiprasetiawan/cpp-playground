#pragma once

#include <array>
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
  int                      width_;
  int                      height_;
  inline static Display*   instance_ = nullptr;
  inline static std::mutex mtx_m, data_mtx;
  inline static ANSI&      ansiInstance = ANSI::getInstance();
  std::array<int, 3>       backgroundRGB, foregroundRGB;
  std::vector<char>        data;
  std::thread              runner;

  // private constructor
  Display() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) width_ = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    else width_ = 80;  // fallback
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) width_ = w.ws_col;
    else width_ = 80;  // fallback
#endif
    height_ = width_;  // square
  }

 public:
  Display(const Display&)            = delete;
  Display& operator=(const Display&) = delete;

  static Display& getInstance() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!instance_) instance_ = new Display();
    return *instance_;
  }

  int                width() const { return width_; }
  int                height() const { return height_; }
  ANSI&              getANSI() { return ansiInstance; }
  std::array<int, 3> get_background_RGB() { return backgroundRGB; }
  std::array<int, 3> get_foreground_RGB() { return foregroundRGB; }

#define setter_RGB(name)                 \
  void set_##name(int r, int g, int b) { \
    name##RGB[0] = r;                    \
    name##RGB[1] = g;                    \
    name##RGB[2] = b;                    \
  }

  setter_RGB(background);
  setter_RGB(foreground);

  bool push_buffer(std::vector<char> buffer) {
    if (data.size() + buffer.size() > width_ * height_) return false;
    for (auto c : buffer) data.push_back(c);
    return true;
  }
};

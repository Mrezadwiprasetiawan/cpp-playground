#pragma once

#include <array>
#include <exception>
#include <initializer_list>
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
  int                                          width_;
  int                                          height_;
  inline static Display*                       instance_;
  inline static std::mutex                     data_mtx;
  inline static ANSI&                          ansiInstance = ANSI::getInstance();
  std::vector<std::vector<std::array<int, 3>>> backgroundRGB{}, foregroundRGB{};
  std::vector<std::vector<char>>               data;
  std::thread                                  runner;

  Display() {
#ifndef _WIN32
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
      width_  = w.ws_col;
      height_ = w.ws_row;
    } else {
      width_  = 80;
      height_ = 24;
    }
#else
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
      width_  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
      height_ = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
      width_  = 80;
      height_ = 24;
    }
#endif
    // resize buffers sekarang setelah width_ dan height_ diketahui
    data.resize(height_, std::vector<char>(width_, ' '));
    backgroundRGB.resize(height_, std::vector<std::array<int, 3>>(width_, {0, 0, 0}));
    foregroundRGB.resize(height_, std::vector<std::array<int, 3>>(width_, {255, 255, 255}));
    ansiInstance.enterAlternateScreen();
  }

 public:
  Display(const Display&)            = delete;
  Display& operator=(const Display&) = delete;

  static Display& getInstance() {
    static Display instance;
    return instance;
  }

  ~Display() { ansiInstance.exitAlternateScreen(); }

  int                                          width() const { return width_; }
  int                                          height() const { return height_; }
  ANSI&                                        getANSI() { return ansiInstance; }
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

#define setter_RGB(name)                        \
  void set_##name(int r, int g, int b) {        \
    using namespace std;                        \
    lock_guard<mutex> lock(data_mtx);           \
    for (auto& rowrgb : name##RGB)              \
      for (auto& rgb : rowrgb) rgb = {r, g, b}; \
  }

  setter_RGB(background);
  setter_RGB(foreground);
#undef setter_RGB

  bool push_buffer(int row, const std::vector<char>& buffer) {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    if (row < 0 || row >= height_ || buffer.size() > (size_t)width_) return false;
    for (size_t i = 0; i < buffer.size(); ++i) data[row][i] = buffer[i];
    return true;
  }

  bool push_buffer(int row, std::initializer_list<char> buffer) {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    if (row < 0 || row >= height_ || buffer.size() > (size_t)width_) return false;
    size_t i = 0;
    for (auto c : buffer) data[row][i++] = c;
    return true;
  }

  bool push_buffer_rgb(const std::vector<std::vector<std::array<int, 3>>>& rgb) {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    if (rgb.empty()) return false;
    const size_t rows = min(rgb.size(), (size_t)height_);
    for (size_t y = 0; y < rows; ++y) {
      const size_t cols = min(rgb[y].size(), (size_t)width_);
      for (size_t x = 0; x < cols; ++x) backgroundRGB[y][x] = rgb[y][x];
    }
    return true;
  }

  std::vector<std::vector<char>> get_frame_buffer() {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    return data;
  }

  bool render() {
    using namespace std;
    lock_guard<mutex> lock(data_mtx);
    ansiInstance.clearScreen();
    try {
      for (int y = 0; y < height_; ++y) {
        // safety: jika data belum berukuran penuh, output spasi
        const bool hasDataRow = (y < (int)data.size());
        const bool hasBgRow   = (y < (int)backgroundRGB.size());
        for (int x = 0; x < width_; ++x) {
          if (hasBgRow && x < (int)backgroundRGB[y].size()) {
            const auto& col = backgroundRGB[y][x];
            ansiInstance.bgRGB(col[0], col[1], col[2]);
          } else ansiInstance.bgRGB(0, 0, 0);

          char ch = ' ';
          if (hasDataRow && x < (int)data[y].size()) ch = data[y][x];
          cout << ch;
        }
        cout << '\n';
      }
      ansiInstance.reset();  // reset warna di akhir frame
      cout.flush();
    } catch (const ios_base::failure& e) {
      cerr << "[Render Error] Output stream failure: " << e.what() << '\n';
      return false;
    } catch (const exception& e) {
      cerr << "[Render Error] " << e.what() << '\n';
      return false;
    } catch (...) {
      cerr << "[Render Error] Unknown error occurred during render.\n";
      return false;
    }
    return true;
  }
};

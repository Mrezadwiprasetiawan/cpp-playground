#pragma once
#include <iostream>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#endif

class ANSI {
 private:
  inline static ANSI*      instance_;
  inline static std::mutex mtx_;

  ANSI() {
#ifdef _WIN32
    // Enable ANSI escape codes on Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
      DWORD dwMode = 0;
      GetConsoleMode(hOut, &dwMode);
      SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
  }

 public:
  ANSI(const ANSI&)            = delete;
  ANSI& operator=(const ANSI&) = delete;

  static ANSI& getInstance() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!instance_) instance_ = new ANSI();
    return *instance_;
  }

  // Cursor
  void clearScreen() { std::cout << "\033[2J"; }
  void moveCursor(int row, int col) { std::cout << "\033[" << row << ";" << col << "H"; }
  void hideCursor() { std::cout << "\033[?25l"; }
  void showCursor() { std::cout << "\033[?25h"; }

  // Text styles
  void bold() { std::cout << "\033[1m"; }
  void underline() { std::cout << "\033[4m"; }
  void reset() { std::cout << "\033[0m"; }

  // Colors (foreground)
  void fgBlack() { std::cout << "\033[30m"; }
  void fgRed() { std::cout << "\033[31m"; }
  void fgGreen() { std::cout << "\033[32m"; }
  void fgYellow() { std::cout << "\033[33m"; }
  void fgBlue() { std::cout << "\033[34m"; }
  void fgMagenta() { std::cout << "\033[35m"; }
  void fgCyan() { std::cout << "\033[36m"; }
  void fgWhite() { std::cout << "\033[37m"; }

  // Colors (background)
  void bgBlack() { std::cout << "\033[40m"; }
  void bgRed() { std::cout << "\033[41m"; }
  void bgGreen() { std::cout << "\033[42m"; }
  void bgYellow() { std::cout << "\033[43m"; }
  void bgBlue() { std::cout << "\033[44m"; }
  void bgMagenta() { std::cout << "\033[45m"; }
  void bgCyan() { std::cout << "\033[46m"; }
  void bgWhite() { std::cout << "\033[47m"; }

  // 256-color support
  void fg256(int code) { std::cout << "\033[38;5;" << code << "m"; }
  void bg256(int code) { std::cout << "\033[48;5;" << code << "m"; }

  // True color (RGB)
  void fgRGB(int r, int g, int b) { std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m"; }
  void bgRGB(int r, int g, int b) { std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m"; }
};
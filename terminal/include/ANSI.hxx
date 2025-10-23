#pragma once

#include <algorithm>  // for std::clamp
#include <iostream>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#endif

// Macro untuk aman dari ios_base::failure dan auto-flush
#define ANSI_SAFE(expr)                                                                             \
  do {                                                                                              \
    try {                                                                                           \
      expr;                                                                                         \
      std::cout << std::flush;                                                                      \
    } catch (const std::ios_base::failure& e) { std::cerr << "[ANSI Error] " << e.what() << '\n'; } \
  } while (0)

class ANSI {
  ANSI() {
#ifdef _WIN32
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
    static ANSI instance;
    return instance;
  }

  void enterAlternateScreen() { ANSI_SAFE(std::cout << "\033[?1049h" << std::flush); }
  void exitAlternateScreen() { ANSI_SAFE(std::cout << "\033[?1049l" << std::flush); }

  // Cursor
  void clearScreen() { ANSI_SAFE(std::cout << "\033[2J\033[H" << std::flush); }
  void moveCursor(int row, int col) { ANSI_SAFE(std::cout << "\033[" << row << ";" << col << "H" << std::flush); }
  void hideCursor() { ANSI_SAFE(std::cout << "\033[?25l" << std::flush); }
  void showCursor() { ANSI_SAFE(std::cout << "\033[?25h" << std::flush); }

  // Text styles
  void bold() { ANSI_SAFE(std::cout << "\033[1m"); }
  void underline() { ANSI_SAFE(std::cout << "\033[4m"); }
  void reset() { ANSI_SAFE(std::cout << "\033[0m"); }

  // Colors (foreground)
  void fgBlack() { ANSI_SAFE(std::cout << "\033[30m"); }
  void fgRed() { ANSI_SAFE(std::cout << "\033[31m"); }
  void fgGreen() { ANSI_SAFE(std::cout << "\033[32m"); }
  void fgYellow() { ANSI_SAFE(std::cout << "\033[33m"); }
  void fgBlue() { ANSI_SAFE(std::cout << "\033[34m"); }
  void fgMagenta() { ANSI_SAFE(std::cout << "\033[35m"); }
  void fgCyan() { ANSI_SAFE(std::cout << "\033[36m"); }
  void fgWhite() { ANSI_SAFE(std::cout << "\033[37m"); }

  // Colors (background)
  void bgBlack() { ANSI_SAFE(std::cout << "\033[40m"); }
  void bgRed() { ANSI_SAFE(std::cout << "\033[41m"); }
  void bgGreen() { ANSI_SAFE(std::cout << "\033[42m"); }
  void bgYellow() { ANSI_SAFE(std::cout << "\033[43m"); }
  void bgBlue() { ANSI_SAFE(std::cout << "\033[44m"); }
  void bgMagenta() { ANSI_SAFE(std::cout << "\033[45m"); }
  void bgCyan() { ANSI_SAFE(std::cout << "\033[46m"); }
  void bgWhite() { ANSI_SAFE(std::cout << "\033[47m"); }

  // 256-color support
  void fg256(int code) { ANSI_SAFE(std::cout << "\033[38;5;" << code << "m"); }
  void bg256(int code) { ANSI_SAFE(std::cout << "\033[48;5;" << code << "m"); }

  // True color (RGB)
  void fgRGB(int r, int g, int b) {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);
    ANSI_SAFE(std::cout << "\033[38;2;" << r << ";" << g << ";" << b << "m");
  }

  void bgRGB(int r, int g, int b) {
    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);
    ANSI_SAFE(std::cout << "\033[48;2;" << r << ";" << g << ";" << b << "m");
  }
};

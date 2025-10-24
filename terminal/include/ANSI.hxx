#pragma once

#include <algorithm>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

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

  // Screen
  void enterAlternateScreen() { ANSI_SAFE(std::cout << "\033[?1049h"); }
  void exitAlternateScreen() { ANSI_SAFE(std::cout << "\033[?1049l"); }
  void clearScreen() { ANSI_SAFE(std::cout << "\033[2J\033[H"); }
  void scrollUp(int n = 1) { ANSI_SAFE(std::cout << "\033[" << n << "S"); }
  void scrollDown(int n = 1) { ANSI_SAFE(std::cout << "\033[" << n << "T"); }

  // Cursor
  void moveCursor(int row, int col) { ANSI_SAFE(std::cout << "\033[" << row << ";" << col << "H"); }
  void cursorUp(int n = 1) { ANSI_SAFE(std::cout << "\033[" << n << "A"); }
  void cursorDown(int n = 1) { ANSI_SAFE(std::cout << "\033[" << n << "B"); }
  void cursorForward(int n = 1) { ANSI_SAFE(std::cout << "\033[" << n << "C"); }
  void cursorBack(int n = 1) { ANSI_SAFE(std::cout << "\033[" << n << "D"); }
  void hideCursor() { ANSI_SAFE(std::cout << "\033[?25l"); }
  void showCursor() { ANSI_SAFE(std::cout << "\033[?25h"); }
  void saveCursor() { ANSI_SAFE(std::cout << "\033[s"); }
  void restoreCursor() { ANSI_SAFE(std::cout << "\033[u"); }

  // Line erase
  void eraseLine() { ANSI_SAFE(std::cout << "\033[2K"); }
  void eraseToEnd() { ANSI_SAFE(std::cout << "\033[K"); }

  // Text styles
  void bold() { ANSI_SAFE(std::cout << "\033[1m"); }
  void underline() { ANSI_SAFE(std::cout << "\033[4m"); }
  void blink() { ANSI_SAFE(std::cout << "\033[5m"); }
  void reverse() { ANSI_SAFE(std::cout << "\033[7m"); }
  void conceal() { ANSI_SAFE(std::cout << "\033[8m"); }
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

  // Hyperlink (some terminals)
  void hyperlink(const std::string& url, const std::string& text) { ANSI_SAFE(std::cout << "\033]8;;" << url << "\033\\" << text << "\033]8;;\033\\"); }
};

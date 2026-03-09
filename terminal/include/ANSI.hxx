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

#include <algorithm>
#include <cstdint>
#include <string>

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

class ANSI {
  std::string buf_;

  void appendByte(uint8_t n) { writeByte(buf_, n); }

 public:
  /* ── Static write helpers — thread-safe, tulis ke string eksternal ────────
   * Karena parameternya std::string& milik caller (bukan buf_), fungsi ini
   * aman dipanggil dari banyak thread sekaligus tanpa mutex.                 */
  static void writeByte(std::string& s, uint8_t n) {
    if (n >= 100) {
      s += static_cast<char>('0' + n / 100);
      s += static_cast<char>('0' + (n / 10) % 10);
      s += static_cast<char>('0' + n % 10);
    } else if (n >= 10) {
      s += static_cast<char>('0' + n / 10);
      s += static_cast<char>('0' + n % 10);
    } else {
      s += static_cast<char>('0' + n);
    }
  }

  static void writeBgRGB(std::string& s, uint8_t r, uint8_t g, uint8_t b) {
    s += "\033[48;2;";
    writeByte(s, r); s += ';';
    writeByte(s, g); s += ';';
    writeByte(s, b); s += 'm';
  }

  static void writeFgRGB(std::string& s, uint8_t r, uint8_t g, uint8_t b) {
    s += "\033[38;2;";
    writeByte(s, r); s += ';';
    writeByte(s, g); s += ';';
    writeByte(s, b); s += 'm';
  }

 private:

  /* Untuk angka umum (baris/kolom) — tidak di hot path */
  void appendInt(int n) {
    if (n == 0) { buf_ += '0'; return; }
    char tmp[12]; int i = 12;
    while (n > 0) { tmp[--i] = static_cast<char>('0' + n % 10); n /= 10; }
    buf_.append(tmp + i, static_cast<std::size_t>(12 - i));
  }

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

  /* Write seluruh buffer ke stdout sekaligus via raw write, lalu kosongkan */
  void flush() {
    if (buf_.empty()) return;
#ifdef _WIN32
    _write(1, buf_.data(), static_cast<unsigned>(buf_.size()));
#else
    write(STDOUT_FILENO, buf_.data(), buf_.size());
#endif
    buf_.clear();
  }

  void append(std::string_view sv) { buf_.append(sv.data(), sv.size()); }
  void append(char c)              { buf_ += c; }
  void reserve(std::size_t n)      { buf_.reserve(n); }

  // ── Screen ────────────────────────────────────────────────────────────────
  void enterAlternateScreen() { buf_ += "\033[?1049h"; flush(); }
  void exitAlternateScreen()  { buf_ += "\033[?1049l"; flush(); }
  void clearScreen()          { buf_ += "\033[2J\033[H"; }
  /* Pindah ke home tanpa clear — cukup untuk overwrite frame berikutnya */
  void homeCursor()           { buf_ += "\033[H"; }
  void scrollUp(int n = 1)    { buf_ += "\033["; appendInt(n); buf_ += 'S'; }
  void scrollDown(int n = 1)  { buf_ += "\033["; appendInt(n); buf_ += 'T'; }

  // ── Cursor ────────────────────────────────────────────────────────────────
  void moveCursor(int row, int col) {
    buf_ += "\033["; appendInt(row); buf_ += ';'; appendInt(col); buf_ += 'H';
  }
  void cursorUp(int n = 1)      { buf_ += "\033["; appendInt(n); buf_ += 'A'; }
  void cursorDown(int n = 1)    { buf_ += "\033["; appendInt(n); buf_ += 'B'; }
  void cursorForward(int n = 1) { buf_ += "\033["; appendInt(n); buf_ += 'C'; }
  void cursorBack(int n = 1)    { buf_ += "\033["; appendInt(n); buf_ += 'D'; }
  void hideCursor()             { buf_ += "\033[?25l"; }
  void showCursor()             { buf_ += "\033[?25h"; }
  void saveCursor()             { buf_ += "\033[s"; }
  void restoreCursor()          { buf_ += "\033[u"; }

  // ── Line erase ────────────────────────────────────────────────────────────
  void eraseLine()  { buf_ += "\033[2K"; }
  void eraseToEnd() { buf_ += "\033[K"; }

  // ── Text styles ───────────────────────────────────────────────────────────
  void bold()      { buf_ += "\033[1m"; }
  void underline() { buf_ += "\033[4m"; }
  void blink()     { buf_ += "\033[5m"; }
  void reverse()   { buf_ += "\033[7m"; }
  void conceal()   { buf_ += "\033[8m"; }
  void reset()     { buf_ += "\033[0m"; }

  // ── Foreground colors ─────────────────────────────────────────────────────
  void fgBlack()   { buf_ += "\033[30m"; }
  void fgRed()     { buf_ += "\033[31m"; }
  void fgGreen()   { buf_ += "\033[32m"; }
  void fgYellow()  { buf_ += "\033[33m"; }
  void fgBlue()    { buf_ += "\033[34m"; }
  void fgMagenta() { buf_ += "\033[35m"; }
  void fgCyan()    { buf_ += "\033[36m"; }
  void fgWhite()   { buf_ += "\033[37m"; }

  // ── Background colors ─────────────────────────────────────────────────────
  void bgBlack()   { buf_ += "\033[40m"; }
  void bgRed()     { buf_ += "\033[41m"; }
  void bgGreen()   { buf_ += "\033[42m"; }
  void bgYellow()  { buf_ += "\033[43m"; }
  void bgBlue()    { buf_ += "\033[44m"; }
  void bgMagenta() { buf_ += "\033[45m"; }
  void bgCyan()    { buf_ += "\033[46m"; }
  void bgWhite()   { buf_ += "\033[47m"; }

  // ── 256-color ─────────────────────────────────────────────────────────────
  void fg256(int code) { buf_ += "\033[38;5;"; appendInt(code); buf_ += 'm'; }
  void bg256(int code) { buf_ += "\033[48;5;"; appendInt(code); buf_ += 'm'; }

  // ── True color RGB — delegate ke static write helpers ────────────────────
  void fgRGB(uint8_t r, uint8_t g, uint8_t b) { writeFgRGB(buf_, r, g, b); }
  void bgRGB(uint8_t r, uint8_t g, uint8_t b) { writeBgRGB(buf_, r, g, b); }

  /* Overload int — clamp lalu forward ke uint8_t */
  void fgRGB(int r, int g, int b) {
    fgRGB(static_cast<uint8_t>(std::clamp(r, 0, 255)),
          static_cast<uint8_t>(std::clamp(g, 0, 255)),
          static_cast<uint8_t>(std::clamp(b, 0, 255)));
  }
  void bgRGB(int r, int g, int b) {
    bgRGB(static_cast<uint8_t>(std::clamp(r, 0, 255)),
          static_cast<uint8_t>(std::clamp(g, 0, 255)),
          static_cast<uint8_t>(std::clamp(b, 0, 255)));
  }

  // ── Hyperlink ─────────────────────────────────────────────────────────────
  void hyperlink(const std::string& url, const std::string& text) {
    buf_ += "\033]8;;"; buf_ += url; buf_ += "\033\\";
    buf_ += text;
    buf_ += "\033]8;;\033\\";
  }
};

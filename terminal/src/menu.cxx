#include <termios.h>
#include <unistd.h>
#include <stdio.h>

int main() {
  termios oldt, raw;
  tcgetattr(0, &oldt);
  raw = oldt;
  raw.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(0, TCSANOW, &raw);

  write(1, "\033[?25l", 6); // hide cursor

  while (1) {
    unsigned char c;
    read(0, &c, 1);

    if (c == 0x1B) { // ESC
      unsigned char seq[2];
      if (read(0, seq, 2) == 2) {
        // contoh: arrow keys
        if (seq[0] == '[') {
          if (seq[1] == 'A') printf("UP\n");
          else if (seq[1] == 'B') printf("DOWN\n");
          else if (seq[1] == 'C') printf("RIGHT\n");
          else if (seq[1] == 'D') printf("LEFT\n");
        }
      }
    }

    if (c == 'q') break;
  }

  write(1, "\033[?25h", 6); // show cursor
  tcsetattr(0, TCSANOW, &oldt);
  return 0;
}

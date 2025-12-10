#include <chrono>
#include <thread>
#include <cmath>
#include "display-term.hxx"

struct Body {
  double x;
  double v;
  double m;
  char symbol;

  double prevx;
};

int main() {
  Display &disp = Display::getInstance();
  int W = disp.get_width();
  int H = disp.get_height();

  int mid = H / 2;

  Body small{8, 0, 1, 'o', 8};
  Body big{35, -10, 10000, 'O', 35};

  const int wallX = 2;
  const double dt = 0.05;

  long collisionCountSmallWall = 0;
  long collisionCountSmallBig = 0;

  auto &fb = disp.get_frame_buffer();

  while (true) {
    small.prevx = small.x;
    big.prevx = big.x;

    small.x += small.v * dt;
    big.x += big.v * dt;

    // tumbukan kecil-dinding
    if (small.x <= wallX) {
      small.x = wallX;
      small.v = -small.v;
      collisionCountSmallWall++;
    }

    // tumbukan big-small
    if (big.x <= small.x + 1) {
      double u1 = small.v;
      double u2 = big.v;

      double m1 = small.m;
      double m2 = big.m;

      small.v = (u1 * (m1 - m2) + 2 * m2 * u2) / (m1 + m2);
      big.v = (u2 * (m2 - m1) + 2 * m1 * u1) / (m1 + m2);

      big.x = small.x + 1;

      collisionCountSmallBig++;
    }

    int pxSmall = (int)small.prevx;
    int pxBig   = (int)big.prevx;

    if (pxSmall >= 0 && pxSmall < W) fb[mid][pxSmall] = ' ';
    if (pxBig   >= 0 && pxBig < W) fb[mid][pxBig]   = ' ';

    fb[mid][wallX] = '|';

    int xs = (int)small.x;
    int xb = (int)big.x;

    if (xs >= 0 && xs < W) fb[mid][xs] = small.symbol;
    if (xb >= 0 && xb < W) fb[mid][xb] = big.symbol;

    std::string hud;
    hud += "Mass small=" + std::to_string((int)small.m);
    hud += "  Mass big=" + std::to_string((int)big.m);
    hud += "  v_small=" + std::to_string(small.v);
    hud += "  v_big=" + std::to_string(big.v);
    hud += "  C(small-wall)=" + std::to_string(collisionCountSmallWall);
    hud += "  C(big-small)=" + std::to_string(collisionCountSmallBig);

    for (int i = 0; i < (int)hud.size() && i < W; i++)
      fb[0][i] = hud[i];

    for (int i = hud.size(); i < W; i++)
      fb[0][i] = ' ';

    disp.render();

    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }
}

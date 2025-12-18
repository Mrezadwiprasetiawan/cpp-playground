#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <csignal>
#include "display-term.hxx"

struct Body {
  double pos;
  double vel;
  double mass;
  char symbol;
  double prevPos;
};

volatile std::sig_atomic_t stopFlag = 0;
void handleSigInt(int) { stopFlag = 1; }

int main() {
  std::signal(SIGINT, handleSigInt);

  Display &disp = Display::getInstance();
  int W = disp.get_width();
  int H = disp.get_height();
  int mid = H / 2;

  Body small{8.0, 0.0, 1.0, 'o', 8.0};
  Body big{35.0, -10.0, 1000000.0, 'O', 35.0};

  const int wallX = 2;
  const double dt = 0.016;
  const double EPS = 1e-9;
  const double LARGE_TIME = 1e30;

  long totalCollision = 0;
  auto &fb = disp.get_frame_buffer();

  while (!stopFlag) {
    // Simpan posisi lama
    int prevSmallPos = static_cast<int>(std::floor(small.prevPos));
    int prevBigPos   = static_cast<int>(std::floor(big.prevPos));

    double remainingTime = dt;
    while (remainingTime > EPS) {
      // Waktu tumbukan small-wall
      double tWall = LARGE_TIME;
      if (small.vel < 0) tWall = (wallX + 1.0 - small.pos)/small.vel;

      // Waktu tumbukan small-big
      double dv = big.vel - small.vel;
      double tBS = LARGE_TIME;
      if (std::abs(dv) > EPS) {
        double t = (small.pos - big.pos + 1.0)/dv;
        if (t > EPS) tBS = t;
      }

      double tNext = std::min({tWall, tBS, remainingTime});

      small.pos += small.vel * tNext;
      big.pos   += big.vel * tNext;

      if (std::abs(tNext - tWall) < EPS) {
        small.vel = -small.vel;
        totalCollision++;
      }
      if (std::abs(tNext - tBS) < EPS) {
        double u1 = small.vel, u2 = big.vel;
        double m1 = small.mass, m2 = big.mass;
        small.vel = (u1*(m1-m2) + 2*m2*u2)/(m1+m2);
        big.vel   = (u2*(m2-m1) + 2*m1*u1)/(m1+m2);
        totalCollision++;
      }

      remainingTime -= tNext;
    }

    // Bersihkan posisi lama setelah update
    if (prevSmallPos >= 0 && prevSmallPos < W) fb[mid][prevSmallPos] = ' ';
    if (prevBigPos   >= 0 && prevBigPos   < W) fb[mid][prevBigPos]   = ' ';

    // Perbarui posisi sebelumnya
    small.prevPos = small.pos;
    big.prevPos   = big.pos;

    // Dinding
    if (wallX >= 0 && wallX < W) fb[mid][wallX] = '|';

    // Render benda
    int xs = static_cast<int>(std::floor(small.pos));
    int xb = static_cast<int>(std::floor(big.pos));
    if (xs >= 0 && xs < W) fb[mid][xs] = small.symbol;
    if (xb >= 0 && xb < W) fb[mid][xb] = big.symbol;

    // HUD
    std::string hud;
    hud.reserve(100);
    hud += "Mass small=" + std::to_string(static_cast<int>(small.mass));
    hud += "  Mass big="   + std::to_string(static_cast<int>(big.mass));
    hud += "  v_small="    + std::to_string(small.vel);
    hud += "  v_big="      + std::to_string(big.vel);
    hud += "  Collisions=" + std::to_string(totalCollision);

    for (size_t i = 0; i < hud.size() && i < (size_t)W; ++i)
      fb[0][i] = hud[i];
    for (size_t i = hud.size(); i < (size_t)W; ++i)
      fb[0][i] = ' ';

    disp.render();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  return 0;
}

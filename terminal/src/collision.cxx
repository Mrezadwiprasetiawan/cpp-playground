#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <csignal>
#include <string>
#include "display-term.hxx"

struct Body {
  double pos;
  double vel;
  double mass;
  char symbol;
  double prevPos;
};

volatile sig_atomic_t stopFlag = 0;
void handleSigInt(int) { stopFlag = 1; }

int main(int argc, char **argv) {
  using namespace std;
  signal(SIGINT, handleSigInt);

  Display &disp = Display::getInstance();
  int W = disp.get_width();
  int H = disp.get_height();
  int mid = H / 2;


  char s_sym = 'o', b_sym  = 'O';
  double p_small = 8.0 , m_small = 1.0, v_small = 0, p_big = mid, m_big = 10000, v_big = -10;

  if(argc < 9) {
    cout << "you can use " << argv[0] << "<c_small> <p_small> <m_small> <v_small> <c_cmall> <p_big> <m_big> <v_big> to set manually" << endl;
    cout << "ENTER to continue...";
    while(true) if(cin.get() == '\n') break;
  }
  else {
    s_sym   = argv[1][0];
    p_small = atof(argv[2]);
    m_small = atof(argv[3]);
    v_small = atof(argv[4]);
    b_sym   = argv[5][0];
    p_big   = atof(argv[6]);
    m_big   = atof(argv[7]);
    v_big   = atof(argv[8]);
  }

  Body small{p_small, v_small, m_small, s_sym, p_small};
  Body big{p_big, v_big, m_big, b_sym, p_big};

  const int wallX = 2;
  const double dt = 0.016;
  const double EPS = 1e-9;
  const double LARGE_TIME = 1e30;

  long totalCollision = 0;
  auto &fb = disp.get_frame_buffer();

  while (!stopFlag) {
    // Simpan posisi lama
    int prevSmallPos = static_cast<int>(floor(small.prevPos));
    int prevBigPos   = static_cast<int>(floor(big.prevPos));

    double remainingTime = dt;
    while (remainingTime > EPS) {
      // Waktu tumbukan small-wall
      double tWall = LARGE_TIME;
      if (small.vel < 0) tWall = (wallX + 1.0 - small.pos)/small.vel;

      // Waktu tumbukan small-big
      double dv = big.vel - small.vel;
      double tBS = LARGE_TIME;
      if (abs(dv) > EPS) {
        double t = (small.pos - big.pos + 1.0)/dv;
        if (t > EPS) tBS = t;
      }

      double tNext = min({tWall, tBS, remainingTime});

      small.pos += small.vel * tNext;
      big.pos   += big.vel * tNext;

      if (abs(tNext - tWall) < EPS) {
        small.vel = -small.vel;
        totalCollision++;
      }
      if (abs(tNext - tBS) < EPS) {
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
    int xs = static_cast<int>(floor(small.pos));
    int xb = static_cast<int>(floor(big.pos));
    if (xs >= 0 && xs < W) fb[mid][xs] = small.symbol;
    if (xb >= 0 && xb < W) fb[mid][xb] = big.symbol;

    // HUD
    string hud;
    hud.reserve(100);
    hud += "Mass small=" + to_string(static_cast<int>(small.mass));
    hud += "  Mass big="   + to_string(static_cast<int>(big.mass));
    hud += "  v_small="    + to_string(small.vel);
    hud += "  v_big="      + to_string(big.vel);
    hud += "  Collisions=" + to_string(totalCollision);

    for (size_t i = 0; i < hud.size() && i < (size_t)W; ++i)
      fb[0][i] = hud[i];
    for (size_t i = hud.size(); i < (size_t)W; ++i)
      fb[0][i] = ' ';

    disp.render();
    this_thread::sleep_for(chrono::milliseconds(16));
  }

  return 0;
}

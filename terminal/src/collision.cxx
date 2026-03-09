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

#include <chrono>
#include <cmath>
#include <cstdio>
#include <csignal>
#include <string>
#include <thread>

#include "display-term.hxx"

struct Body {
  double pos;
  double vel;
  double mass;
  char   symbol;
  double prevPos;
};

volatile sig_atomic_t stopFlag = 0;
void handleSigInt(int) { stopFlag = 1; }

int main(int argc, char** argv) {
  using namespace std;

  /* Parsing argumen SEBELUM Display::getInstance() agar output tidak masuk
   * ke alternate screen. Display constructor langsung enterAlternateScreen(). */
  char   s_sym  = 'o', b_sym  = 'O';
  double p_small = 8.0, m_small = 1.0,     v_small = 0.0;
  double p_big   = -1,  m_big   = 10000.0, v_big   = -10.0;  /* p_big diisi setelah dapat H */

  if (argc >= 9) {
    s_sym   = argv[1][0];
    p_small = atof(argv[2]);
    m_small = atof(argv[3]);
    v_small = atof(argv[4]);
    b_sym   = argv[5][0];
    p_big   = atof(argv[6]);
    m_big   = atof(argv[7]);
    v_big   = atof(argv[8]);
  } else {
    printf("Usage: %s <c_small> <p_small> <m_small> <v_small> <c_big> <p_big> <m_big> <v_big>\n",
           argv[0]);
    printf("ENTER to continue with defaults...");
    fflush(stdout);
    while (getchar() != '\n');
  }

  signal(SIGINT, handleSigInt);

  Display& disp = Display::getInstance();
  int      W    = disp.get_width();
  int      H    = disp.get_height();
  int      mid  = H / 2;

  /* p_big default: sepertiga dari lebar terminal */
  if (p_big < 0) p_big = W / 3.0;

  Body small{p_small, v_small, m_small, s_sym, p_small};
  Body big  {p_big,   v_big,   m_big,   b_sym, p_big  };

  const int    wallX     = 2;
  const double dt        = 0.016;
  const double EPS       = 1e-9;
  const double LARGE_T   = 1e30;

  long totalCollision = 0;

  while (!stopFlag) {
    /* Re-fetch setiap frame — handle_resize() di dalam render() bisa
     * realokasi vector (data.assign()), membuat referensi lama invalid. */
    int new_W = disp.get_width();
    int new_H = disp.get_height();
    if (new_W != W || new_H != H) {
      W   = new_W;
      H   = new_H;
      mid = H / 2;
      /* Reset prevPos ke posisi saat ini agar clear frame tidak OOB */
      small.prevPos = small.pos;
      big.prevPos   = big.pos;
    }

    auto& fb = disp.get_frame_buffer(); /* ambil referensi segar tiap frame */

    /* ── Fisika: sub-stepping dalam satu frame dt ──────────────────────── */
    int prevSmallX = static_cast<int>(floor(small.prevPos));
    int prevBigX   = static_cast<int>(floor(big.prevPos));

    double rem = dt;
    while (rem > EPS) {
      /* Waktu tumbukan small ↔ dinding kiri.
       * Dinding di kolom wallX, objek lebar 1 → bouncing saat small.pos = wallX+1 */
      double tWall = LARGE_T;
      if (small.vel < 0)
        tWall = (wallX + 1.0 - small.pos) / small.vel;

      /* Waktu tumbukan small ↔ big.
       * big.pos = small.pos + 1  →  t = (small.pos - big.pos + 1) / (big.vel - small.vel) */
      double tBS = LARGE_T;
      double dv  = big.vel - small.vel;
      if (dv < -EPS) {                    /* big mendekati small dari kanan */
        double t = (small.pos - big.pos + 1.0) / dv;
        if (t > EPS) tBS = t;
      }

      double tNext = min({tWall, tBS, rem});

      small.pos += small.vel * tNext;
      big.pos   += big.vel   * tNext;

      if (tNext >= tWall - EPS && tWall < rem - EPS) {
        /* Tumbukan dinding: small berbalik */
        small.vel = -small.vel;
        ++totalCollision;
      } else if (tNext >= tBS - EPS && tBS < rem - EPS) {
        /* Tumbukan elastis small ↔ big — ELSE IF: tidak boleh keduanya
         * aktif sekaligus; saat tWall == tBS cukup proses salah satu. */
        double u1 = small.vel, u2 = big.vel;
        double m1 = small.mass, m2 = big.mass;
        small.vel = (u1 * (m1 - m2) + 2.0 * m2 * u2) / (m1 + m2);
        big.vel   = (u2 * (m2 - m1) + 2.0 * m1 * u1) / (m1 + m2);
        ++totalCollision;
      }

      rem -= tNext;
    }

    /* ── Render ────────────────────────────────────────────────────────── */
    /* Hapus posisi lama */
    if (prevSmallX >= 0 && prevSmallX < W) fb[mid][prevSmallX] = ' ';
    if (prevBigX   >= 0 && prevBigX   < W) fb[mid][prevBigX]   = ' ';

    /* Update prevPos */
    small.prevPos = small.pos;
    big.prevPos   = big.pos;

    /* Dinding */
    if (wallX >= 0 && wallX < W) fb[mid][wallX] = '|';

    /* Benda */
    int xs = static_cast<int>(floor(small.pos));
    int xb = static_cast<int>(floor(big.pos));
    if (xs >= 0 && xs < W) fb[mid][xs] = small.symbol;
    if (xb >= 0 && xb < W) fb[mid][xb] = big.symbol;

    /* HUD baris 0 */
    string hud;
    hud.reserve(128);
    hud += "small m=";  hud += to_string(static_cast<long long>(small.mass));
    hud += " v=";       hud += to_string(small.vel).substr(0, 7);
    hud += "  big m=";  hud += to_string(static_cast<long long>(big.mass));
    hud += " v=";       hud += to_string(big.vel).substr(0, 7);
    hud += "  col=";    hud += to_string(totalCollision);

    for (int i = 0; i < W; ++i)
      fb[0][i] = (i < static_cast<int>(hud.size())) ? hud[i] : ' ';

    disp.render();
    this_thread::sleep_for(chrono::milliseconds(16));
  }

  return 0;
}

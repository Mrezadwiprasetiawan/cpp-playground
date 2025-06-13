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

#include <cmath>
#include <cstring>
#include <discrete/include/prime.hxx>
#include <iostream>
#include <png.hxx>
#include <string>
#include <vector>

uint64_t WIDTH, HEIGHT, AREA;

uint64_t to_number_with_suffix(const char *str) {
  using namespace std;
  size_t len = strlen(str);
  uint64_t num = 0;

  for (uint64_t i = 0; i < len; ++i) {
    char c = str[i];
    if (c >= '0' && c <= '9') num = num * 10 + (c - '0');
    else {
      char suffix = tolower(str[i]);
      switch (suffix) {
        case 'k': return num << 10;
        case 'm': return num << 20;
        case 'g': return num << 30;
        case 't': return num << 40;
        case 'p': return num << 50;
        case 'e': return num << 60;
        default: std::cerr << "Invalid suffix: " << str[i] << endl; exit(1);
      }
    }
  }
  return num;
}

int main(int argc, const char *argv[]) {
  using namespace Discrete;
  if (argc < 2 || argc > 3) {
    std::cerr << argv[0] << " <WIDTH> <HEIGHT>" << std::endl;
    exit(1);
  }
  if (argc >= 2) WIDTH = to_number_with_suffix(argv[1]);
  if (argc == 3) HEIGHT = to_number_with_suffix(argv[2]);
  else HEIGHT = WIDTH;
  AREA = WIDTH * HEIGHT;
  std::string filename = "prime-uniform-" + std::to_string(WIDTH) + "-" + std::to_string(HEIGHT) + ".png";
  std::string filename2 = "prime-spiral-" + std::to_string(WIDTH) + "-" + std::to_string(HEIGHT) + ".png";
  PNG writer(filename);
  writer.set_width(WIDTH);
  writer.set_height(HEIGHT);
  writer.set_color_type(PNG_COLOR_TYPE_RGB);
  writer.set_bit_depth(8);

  std::vector<png_byte> data(AREA * 3), data2(AREA * 3);

  Prime<uint64_t> prime;

  /* Solve r untuk setiap i
   *
   * ketika i = s * s gw mau r = halfdiagonal
   * sehingga r = sqrt(2)/2 * s
   * dR linear terhadap i dan s = sqrt(i) sehingga
   * r = dR * sqrt(i)
   * dR = sqrt(2)/2
   */

  /* Solve dTheta
   *
   * anggap r tetap sehingga ada 2pi * r unit titik
   * karena ada 2pi*r titik dan pada titik terakhir theta harus = 2pi
   * maka dTheta = 1/r
   * karena r = dR * sqrt(i) dan dR = sqrt(2)/2 maka
   * dTheta = 2/sqrt(2) * sqrt(i)
   */

  float dR = 0.5 * std::sqrt(2.0f);
  const uint64_t cx = WIDTH / 2;
  const uint64_t cy = HEIGHT / 2;

  for (uint32_t i : prime.from_range_limit(AREA)) {
    const float dTheta = 2 / std::sqrt(2 * i);
    const float r = dR * std::sqrt(i);
    const float theta = dTheta * i;
    const int64_t dx = r * std::cos(theta);
    const int64_t dy = r * std::sin(theta);
    if (std::abs(dx) > WIDTH / 2 || std::abs(dy) > HEIGHT / 2) continue;
    uint64_t x = uint64_t(cx + dx);
    uint64_t y = uint64_t(cy + dy);
    uint64_t n = y * WIDTH + x;
    uint64_t base = (n) * 3;
    data[base + 0] = 0;
    data[base + 1] = 0;
    data[base + 2] = 255;
  }

  writer.set_buffer(data);
  if (writer.write()) std::cout << "berhasil ditulis ke " << filename << std::endl;
  else std::cout << "gagal menulis ke " << filename << std::endl;

  // spiral logarithmic cek ini
  // https://en.wikipedia.org/wiki/Logarithmic_spiral
  const float dTheta = std::log(dR);
  for (uint32_t i : prime.from_range_limit(AREA)) {
    float r = dR * std::sqrt(i);
    float theta = dTheta * i;
    const int64_t dx = r * std::cos(theta);
    const int64_t dy = r * std::sin(theta);
    if (std::abs(dx) > WIDTH / 2 || std::abs(dy) > HEIGHT / 2) continue;
    uint64_t x = uint64_t(cx + dx);
    uint64_t y = uint64_t(cy + dy);
    uint64_t n = y * WIDTH + x;
    uint64_t base = (n) * 3;
    data2[base + 0] = 0;
    data2[base + 1] = 0;
    data2[base + 2] = 255;
  }

  writer.set_filename(filename2);
  writer.set_buffer(data2);
  if (writer.write()) std::cout << "berhasil ditulis ke " << filename2 << std::endl;
  else std::cout << "gagal menulis ke " << filename2 << std::endl;
}

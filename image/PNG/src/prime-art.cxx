#include <cmath>
#include <cstring>
#include <iostream>
#include <numbers.hxx>
#include <png.hxx>
#include <string>
#include <vector>

uint64_t WIDTH, HEIGHT, AREA;

uint64_t to_number_with_suffix(const char* str) {
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

int main(int argc, const char* argv[]) {
  if (argc < 2 || argc > 3) {
    std::cerr << argv[0] << " <WIDTH> <HEIGHT>" << std::endl;
    exit(1);
  }
  if (argc >= 2) WIDTH = to_number_with_suffix(argv[1]);
  if (argc == 3) HEIGHT = to_number_with_suffix(argv[2]);
  else
    HEIGHT = WIDTH;
  AREA = WIDTH * HEIGHT;
  std::string filename =
      "prime-spiral-" + std::to_string(WIDTH) + "-" + std::to_string(HEIGHT) + ".png";
  PNG writer(filename);
  writer.set_width(WIDTH);
  writer.set_height(HEIGHT);
  writer.set_color_type(PNG_COLOR_TYPE_RGB);
  writer.set_bit_depth(8);

  std::vector<png_byte> data(AREA * 3);

  Prime<uint64_t> prime;
  const float scaleR = 0.5 * sqrt(2);
  const float scaleTheta = std::log(scaleR);
  const uint64_t cx = WIDTH / 2;
  const uint64_t cy = HEIGHT / 2;

  for (uint32_t i : prime.from_range_limit(AREA)) {
    float r = scaleR * std::sqrt(i);
    float theta = scaleTheta * i;
    uint64_t x = uint64_t(cx + r * std::cos(theta));
    uint64_t y = uint64_t(cy + r * std::sin(theta));
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
      uint64_t n = y * WIDTH + x;
      uint64_t base = (n) * 3;
      data[base + 0] = 255;
      data[base + 1] = 255;
      data[base + 2] = 255;
    }
  }

  writer.set_buffer(data);
  if (writer.write())
    std::cout << "berhasil ditulis ke " << filename << std::endl;
  else
    std::cout << "gagal menulis ke " << filename << std::endl;
}

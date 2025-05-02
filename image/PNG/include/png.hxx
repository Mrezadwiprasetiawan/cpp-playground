#pragma once

#include <png.h>

#include <fstream>
#include <string>
#include <vector>

class PNG {
 private:
  std::string fileName;
  std::ifstream input;
  std::ofstream output;
  png_structp pngPtr = nullptr;
  png_infop infoPtr = nullptr;
  int imageWidth = 0, imageHeight = 0;
  png_byte imageColorType = 0, imageBitDepth = 8;
  std::vector<png_byte> imageBuffer;

  static void png_write_callback(png_structp png, png_bytep data,
                                 png_size_t length) {
    auto stream = reinterpret_cast<std::ostream *>(png_get_io_ptr(png));
    stream->write(reinterpret_cast<char *>(data), length);
    if (!*stream) png_error(png, "WRITE Error");
  }

  static void png_read_callback(png_structp png, png_bytep data,
                                png_size_t length) {
    auto stream = reinterpret_cast<std::istream *>(png_get_io_ptr(png));
    stream->read(reinterpret_cast<char *>(data), length);
    if (!*stream) png_error(png, "READ Error");
  }

  int get_channel_count() const {
    switch (imageColorType) {
      case PNG_COLOR_TYPE_GRAY: return 1;
      case PNG_COLOR_TYPE_GRAY_ALPHA: return 2;
      case PNG_COLOR_TYPE_RGB: return 3;
      case PNG_COLOR_TYPE_RGBA: return 4;
      default: return 3;
    }
  }

 public:
  PNG(const std::string &fileName) : fileName(fileName) {}

  ~PNG() {
    if (pngPtr) {
      if (input.is_open()) png_destroy_read_struct(&pngPtr, &infoPtr, nullptr);
      if (output.is_open()) png_destroy_write_struct(&pngPtr, &infoPtr);
    }
  }

  bool read() {
    input.open(fileName, std::ios::binary);
    if (!input) return false;

    pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr,
                                    nullptr);
    if (!pngPtr) return false;
    infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr || setjmp(png_jmpbuf(pngPtr))) return false;

    png_set_read_fn(pngPtr, static_cast<void *>(&input), png_read_callback);
    png_read_info(pngPtr, infoPtr);

    imageWidth = png_get_image_width(pngPtr, infoPtr);
    imageHeight = png_get_image_height(pngPtr, infoPtr);
    imageColorType = png_get_color_type(pngPtr, infoPtr);
    imageBitDepth = png_get_bit_depth(pngPtr, infoPtr);

    png_read_update_info(pngPtr, infoPtr);

    int rowBytes = png_get_rowbytes(pngPtr, infoPtr);
    imageBuffer.resize(imageHeight * rowBytes);
    std::vector<png_bytep> rows(imageHeight);
    for (int y = 0; y < imageHeight; y++) rows[y] = &imageBuffer[y * rowBytes];

    png_read_image(pngPtr, rows.data());
    return true;
  }

  bool write() {
    output.open(fileName, std::ios::binary);
    if (!output) return false;

    int rowBytes = imageWidth * get_channel_count();
    if (imageBuffer.size() != static_cast<size_t>(imageHeight * rowBytes))
      imageBuffer.resize(imageHeight * rowBytes);

    pngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr,
                                     nullptr);
    if (!pngPtr) return false;
    infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr || setjmp(png_jmpbuf(pngPtr))) return false;

    png_set_write_fn(pngPtr, static_cast<void *>(&output), png_write_callback,
                     nullptr);
    png_set_IHDR(pngPtr, infoPtr, imageWidth, imageHeight, imageBitDepth,
                 imageColorType, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(pngPtr, infoPtr);

    std::vector<png_bytep> rows(imageHeight);
    for (int y = 0; y < imageHeight; y++) rows[y] = &imageBuffer[y * rowBytes];

    png_write_image(pngPtr, rows.data());
    png_write_end(pngPtr, nullptr);
    return true;
  }

  png_byte &pixel(int x, int y, int channel = 0) {
    return imageBuffer[(y * imageWidth + x) * get_channel_count() + channel];
  }

  // Getters
  int get_width() const { return imageWidth; }
  int get_height() const { return imageHeight; }
  png_byte get_color_type() const { return imageColorType; }
  png_byte get_bit_depth() const { return imageBitDepth; }
  const std::vector<png_byte> &get_buffer() const { return imageBuffer; }

  // Setters
  void set_width(int w) { imageWidth = w; }
  void set_height(int h) { imageHeight = h; }
  void set_color_type(png_byte ct) { imageColorType = ct; }
  void set_bit_depth(png_byte bd) { imageBitDepth = bd; }
  void set_buffer(const std::vector<png_byte> &buf) { imageBuffer = buf; }
};

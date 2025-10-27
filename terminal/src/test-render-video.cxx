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


#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

#include "display-term.hxx"

static bool running = true;
void        handle_sigint(int) { running = false; }

int main(int argc, char* argv[]) {
  signal(SIGINT, handle_sigint);
  using namespace std;
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <video_file>\n";
    return 1;
  }
  string           path = argv[1];
  cv::VideoCapture cap(path);
  if (!cap.isOpened()) {
    cerr << "Failed to open video: " << path << "\n";
    return 1;
  }
  Display&                      display = Display::getInstance();
  int                           termW   = display.get_width();
  int                           termH   = display.get_height();
  vector<vector<array<int, 3>>> rgb(termH, vector<array<int, 3>>(termW, {0, 0, 0}));
  int                           videoW      = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
  int                           videoH      = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  double                        videoAspect = double(videoW) / videoH;
  double                        termAspect  = double(termW) / termH;
  int                           renderW, renderH;
  if (videoAspect > termAspect) {
    renderW = termW;
    renderH = int(termW / videoAspect);
  } else {
    renderH = termH;
    renderW = int(termH * videoAspect);
  }
  int     offsetX = (termW - renderW) / 2;
  int     offsetY = (termH - renderH) / 2;
  cv::Mat frame, resized;
  while (cap.read(frame)) {
    if (!running) break;
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    cv::resize(frame, resized, cv::Size(renderW, renderH), 0, 0, cv::INTER_AREA);
#pragma omp parallel for
    for (int y = 0; y < termH; ++y)
      for (int x = 0; x < termW; ++x) rgb[y][x] = {0, 0, 0};
#pragma omp parallel for
    for (int y = 0; y < renderH; ++y) {
      const cv::Vec3b* ptr = resized.ptr<cv::Vec3b>(y);
      for (int x = 0; x < renderW; ++x) {
        const cv::Vec3b& c            = ptr[x];
        rgb[y + offsetY][x + offsetX] = {c[0], c[1], c[2]};
      }
    }
    display.push_buffer_bg(rgb);
    display.render();
    int delay = (int)(1000.0 / cap.get(cv::CAP_PROP_FPS));
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
  }

  return 0;
}
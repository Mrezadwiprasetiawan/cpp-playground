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

  Display& display = Display::getInstance();
  int      termW   = display.get_width();
  int      termH   = display.get_height();

  // Pre-allocate buffer sekali saja
  vector<vector<array<int, 3>>> rgb(termH, vector<array<int, 3>>(termW));

  cv::Mat frame;
  while (cap.read(frame)) {
    if (!running) break;

    // Resize cepat
    cv::resize(frame, frame, cv::Size(termW, termH), 0, 0, cv::INTER_NEAREST);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

// Ambil pointer langsung
#pragma omp parallel for
    for (int y = 0; y < termH; ++y) {
      const cv::Vec3b* ptr = frame.ptr<cv::Vec3b>(y);
      for (int x = 0; x < termW; ++x) {
        const cv::Vec3b& c = ptr[x];
        rgb[y][x]          = {c[0], c[1], c[2]};
      }
    }

    display.push_buffer_rgb(rgb);
    display.render();

    int delay = (int)(1000.0 / cap.get(cv::CAP_PROP_FPS));
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
  }

  return 0;
}

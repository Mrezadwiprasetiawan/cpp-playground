#include <csignal>
#include <iostream>
#include <opencv2/opencv.hpp>
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

  cv::Mat frame;
  while (cap.read(frame)) {
    if (!running) break;
    cv::resize(frame, frame, cv::Size(termW, termH));
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

    vector<vector<array<int, 3>>> rgb(termH, vector<array<int, 3>>(termW));

    for (int y = 0; y < termH; ++y)
      for (int x = 0; x < termW; ++x) {
        cv::Vec3b c = frame.at<cv::Vec3b>(y, x);
        rgb[y][x]   = {c[0], c[1], c[2]};
      }

    display.push_buffer_rgb(rgb);
    display.render();

    // sesuaikan fps
    int delay = (int)(1000.0 / cap.get(cv::CAP_PROP_FPS));
    if (delay < 10) delay = 10;
    cv::waitKey(delay);
  }

  return 0;
}

#include "Webcam.h"
#include <iostream>

using namespace std;

int main() {
  ct::Webcam w(0);
  w.openStream();

  ct::Webcam x;
  x = w;

  cv::namedWindow("Webcam");
  cv::Mat frame;

  while (true) {
    bool success = x.getFrame(frame);
    if (!success) {
      std::cout << "Cannot read frame from feed" << std::endl;
      break;
    }
    imshow("Webcam", frame);
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }

  return 0;
}
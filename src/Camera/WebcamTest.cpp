#include "Webcam.h"
#include <iostream>

int main() {
  ct::Webcam w(0);
  

  ct::Webcam w2;
  w2 = w;

  w.openStream();
  w2.openStream();

  cv::namedWindow("Webcam");
  cv::Mat frame;

  cv::namedWindow("Webcam2");
  cv::Mat frame2;

  while (true) {
    bool success = w.getFrame(frame);
    bool success2 = w2.getFrame(frame2);
    if (!success) {
      std::cout << "Cannot read frame from feed" << std::endl;
      break;
    }
    imshow("Webcam", frame);
    imshow("Webcam2", frame2);
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }

  return 0;
}

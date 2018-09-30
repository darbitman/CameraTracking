#include "IPCam.h"
#include <iostream>


int main() {
  ct::IPCam cam1("http://webcam01.bigskyresort.com/mjpg/video.mjpg");
  cam1.openStream();
  cv::namedWindow("Live Feed");
  cv::Mat frame;

  while (true) {
    bool success = cam1.getFrame(frame);
    if (!success) {
      std::cout << "Cannot read frame from feed" << std::endl;
      break;
    }
    imshow("Live Feed", frame);
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }

  return 0;
}

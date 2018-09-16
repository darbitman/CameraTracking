#include "IPCam.h"
#include <iostream>


int main() {
  cv::VideoCapture c;
  c.open(1);
  cv::Mat img;
  c.read(img);
  cv::namedWindow("Webcam");

  while (true) {
    bool success = c.read(img);
    if (!success) {
      std::cout << "Cannot read frame from feed" << std::endl;
      break;
    }
    imshow("Webcam", img);
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }
  /*
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
  */
  return 0;
}

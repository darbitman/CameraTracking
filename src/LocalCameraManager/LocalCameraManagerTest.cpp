#include "LocalCameraManager.h"
#include <iostream>

using namespace std;

int main() {
  ct::LocalCameraManager lc;
  cout << lc.getCameraCount();
  ct::Webcam* w;
  lc.getCameraAtIndex(0, w);

  cv::namedWindow("Webcam");
  cv::Mat frame;

  while (true) {
    bool success = w->getFrame(frame);
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
  lc.DeleteCamera(0);
  return 0;
}
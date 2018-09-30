#include "IPCamManager.h"
#include <iostream>

using namespace std;

int main() {
  ct::IPCamManager man1;
  assert(man1.getCameraCount() == 0);
  man1.addCamera("http://webcam01.bigskyresort.com/mjpg/video.mjpg");
  cv::Mat frame;
  cv::namedWindow("Live Feed");
  ct::IPCam cam1;
  assert(man1.getCameraAtIndex(0, cam1) == true);

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

  cv::destroyWindow("Live Feed");
  man1.addCamera("http://webcam01.bigskyresort.com/mjpg/video.mjpg");
  man1.addCamera("http://webcam01.bigskyresort.com/mjpg/video.mjpg");
  assert(man1.getNextCamera(cam1) == true);
  assert(man1.getNextCamera(cam1) == true);
  assert(man1.deleteCamera(2) == true);
  assert(man1.getNextCamera(cam1) == true);
  assert(man1.getNextCamera(cam1) == true);
  return 0;
}
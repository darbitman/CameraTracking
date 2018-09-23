#include <iostream>
#include "LocalCameraManager.h"
#include "Webcam.h"
#include "CannyEdgeDetector.h"
using namespace std;

int main() {
  ct::CannyEdgeDetector ced(100.0, 200.0, 1.3);
  ct::Webcam w(0);
  w.openStream();
  cv::namedWindow("Webcam");
  cv::namedWindow("Edges");


  ct::CannyStruct cs;
  cs.edgeDetectorPtr_ = &ced;
  

  while (true) {
    bool success = w.getFrame(cs.src);
    if (!success) {
      std::cout << "Cannot read frame from feed" << std::endl;
      break;
    }
    ced.RunEdgeDetector(cs);
    imshow("Webcam", cs.src);
    imshow("Edges", cs.detectedEdges);
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }

  return 0;
}

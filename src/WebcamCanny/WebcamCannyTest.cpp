#include <iostream>
#include "LocalCameraManager.h"
#include "Webcam.h"
#include "CannyEdgeDetector.h"
using namespace std;

ct::CannyStruct cs;

void UpdateCannyLowThreshold(int val, void* det) {
  std::cout << "Low threshold: " << val << std::endl;
  ct::CannyEdgeDetector* ced = reinterpret_cast<ct::CannyEdgeDetector*>(det);
  ced->setLowThreshold(val);
  ced->runEdgeDetector(cs);
  imshow("Edges", cs.detectedEdges);
}

void UpdateCannyHighThreshold(int val, void* det) {
  std::cout << "High threshold: " << val << std::endl;
  ct::CannyEdgeDetector* ced = reinterpret_cast<ct::CannyEdgeDetector*>(det);
  ced->setHighThreshold(val);
  ced->runEdgeDetector(cs);
  imshow("Edges", cs.detectedEdges);
}

int main() {
  int lowTh = 100;
  int highTh = 200;
  int maxTh = 255;

  ct::CannyEdgeDetector ced(100.0, 200.0, 3);
  ct::Webcam w(0);
  w.openStream();
  cv::namedWindow("Webcam");
  cv::namedWindow("Edges");
  
  cv::createTrackbar("Low Threshold", "Edges", &lowTh, maxTh, UpdateCannyLowThreshold, &ced);
  cv::createTrackbar("High Threshold", "Edges", &highTh, maxTh, UpdateCannyHighThreshold, &ced);

  while (true) {
    bool success = w.getFrame(cs.src);
    if (!success) {
      std::cout << "Cannot read frame from feed" << std::endl;
      break;
    }
    ced.runEdgeDetector(cs);
    imshow("Webcam", cs.src);
    imshow("Edges", cs.detectedEdges);
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }

  return 0;
}

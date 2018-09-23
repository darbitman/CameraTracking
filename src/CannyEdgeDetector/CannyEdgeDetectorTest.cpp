#include <iostream>
#include "CannyEdgeDetector.h"
ct::CannyStruct cs;

void UpdateCannyLowThreshold(int val, void* det) {
	std::cout << "Low threshold: " << val << std::endl;
	ct::CannyEdgeDetector* ced = reinterpret_cast<ct::CannyEdgeDetector*>(det);
  ced->setLowThreshold(val);
	ced->runEdgeDetector(cs);
	imshow("Filtered Image", cs.detectedEdges);
}

void UpdateCannyHighThreshold(int val, void* det) {
  std::cout << "High threshold: " << val << std::endl;
  ct::CannyEdgeDetector* ced = reinterpret_cast<ct::CannyEdgeDetector*>(det);
  ced->setHighThreshold(val);
  ced->runEdgeDetector(cs);
  imshow("Filtered Image", cs.detectedEdges);
}

int main() {
  int lowTh = 100;
  int highTh = 200;
  int maxTh = 255;

  // read image file
  cv::Mat img = cv::imread("../../../images/guitar.png");

  // check if image was read
  if (img.empty()) {
    std::cout << "Could not open or find the image" << std::endl;
    std::cin.get();  // pause
    return -1;
  }

  cv::namedWindow("Image");
  imshow("Image", img);

  ct::CannyEdgeDetector ced((double)lowTh, (double)highTh);

  cs.src = img;
  ced.runEdgeDetector(cs);
  cv::namedWindow("Filtered Image");
  cv::createTrackbar("Low Threshold", "Filtered Image", &lowTh, maxTh, UpdateCannyLowThreshold, &ced);
  cv::createTrackbar("High Threshold", "Filtered Image", &highTh, maxTh, UpdateCannyHighThreshold, &ced);
  imshow("Filtered Image", cs.detectedEdges);



  while (true) {
	  if (cv::waitKey(30) == 27) {
		  std::cout << "Esc key pressed" << std::endl;
		  break;
	  }
  }
  
  return 0;
}
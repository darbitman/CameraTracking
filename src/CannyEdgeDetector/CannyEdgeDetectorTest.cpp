#include <iostream>
#include "CannyEdgeDetector.h"
ct::CannyStruct cs;

void CannyUpdate(int val, void* det) {
	std::cout << val << std::endl;
	ct::CannyEdgeDetector* ced = reinterpret_cast<ct::CannyEdgeDetector*>(det);
	ced->lowThreshold_ = val;
	ced->RunEdgeDetector(cs);
	imshow("Filtered Image", cs.detectedEdges);
}

int main() {
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

  ct::CannyEdgeDetector ced(100.0, 200.0, 1.3);

  cs.edgeDetector_ = &ced;
  cs.src = img;
  ced.RunEdgeDetector(cs);
  cv::namedWindow("Filtered Image");
  cv::createTrackbar("Ratio: ", "Filtered Image", (int*)&ced.lowThreshold_, ced.maxLowThreshold_, CannyUpdate, &ced);
  imshow("Filtered Image", cs.detectedEdges);



  while (true) {
	  if (cv::waitKey(30) == 27) {
		  std::cout << "Esc key pressed" << std::endl;
		  break;
	  }
  }
  
  return 0;
}
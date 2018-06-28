#include <iostream>
#include "CannyEdgeDetector.h"


int main() {
  // read image file
  cv::Mat img = cv::imread("../../images/guitar.png");

  // check if image was read
  if (img.empty()) {
    std::cout << "Could not open or find the image" << std::endl;
    std::cin.get();  // pause
    return -1;
  }
  
  return 0;
}
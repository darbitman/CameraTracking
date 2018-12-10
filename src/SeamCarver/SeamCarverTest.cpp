#include "SeamCarver.h"
#include "SeamCarverKeepout.h"
#include <iostream>

using namespace std;

int main() {
  cv::Mat img = cv::imread("../../../images/guitar.png", CV_LOAD_IMAGE_COLOR);

  if (img.empty()) {
    std::cout << "Could not open or find the image" << std::endl;
    return -1;
  }
  cv::namedWindow("Image");
  imshow("Image", img);

  ct::SeamCarverKeepout sc1;
  cv::Mat result;
  sc1.setKeepoutRegion(20, 550, 250, 80);
  sc1.findAndRemoveVerticalSeams(50, img, result);

  cv::namedWindow("Result");
  imshow("Result", result);
  
  while (true) {
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }
  return 0;
}
#include "opencv2/opencv.hpp"
#include "SeamCarver.h"
#include "gtest/gtest.h"
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  //testing::InitGoogleTest(&argc, argv);
  //return RUN_ALL_TESTS();
  cv::Mat img = cv::imread("../../../images/guitar.png");

  if (img.empty()) {
    std::cout << "Could not open or find the image" << std::endl;
    return -1;
  }

  cv::namedWindow("Image");
  imshow("Image", img);

  ct::KSeamCarver sc1;
  cv::Mat result;
  sc1.FindAndRemoveVerticalSeams(50, img, result);

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
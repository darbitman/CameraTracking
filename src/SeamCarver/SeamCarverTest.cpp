#include "SeamCarver.h"
#include "SeamCarverKeepout.h"
#include <iostream>
#include <gtest/gtest.h>
using namespace std;

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
  //cv::Mat img = cv::imread("../../../images/guitar.png");

  //if (img.empty()) {
  //  std::cout << "Could not open or find the image" << std::endl;
  //  return -1;
  //}
  //cv::namedWindow("Image");
  //imshow("Image", img);

  //ct::SeamCarverKeepout sc1;
  //cv::Mat result;
  //sc1.setKeepoutRegion(20, 550, 250, 80);
  //sc1.findAndRemoveVerticalSeams(50, img, result);

  //cv::namedWindow("Result");
  //imshow("Result", result);
  //
  //while (true) {
  //  if (cv::waitKey(30) == 27) {
  //    std::cout << "Esc key pressed" << std::endl;
  //    break;
  //  }
  //}
  //return 0;
}
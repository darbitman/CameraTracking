#include "SeamCarver.h"
#include <iostream>

using namespace std;

int main() {
  cv::Mat m(10, 15, CV_32S);
  cout << "height = " << m.size().height << endl;
  cout << "width  = " << m.size().width << endl;
  m.at<int32_t>(9, 14) = -12;
  cout << "row 9, col 14 = " << m.at<int32_t>(9, 14) << endl;

  cv::Mat img = cv::imread("../../../images/guitar.png", CV_LOAD_IMAGE_COLOR);
  cv::Mat bgr[3];
  cv::split(img, bgr);

  if (img.empty()) {
    std::cout << "Could not open or find the image" << std::endl;
    return -1;
  }
  cv::namedWindow("Image");
  imshow("Image", img);

  cv::namedWindow("Blue");
  imshow("Blue", bgr[0]);

  cv::namedWindow("Green");
  imshow("Green", bgr[1]);

  cv::namedWindow("Red");
  imshow("Red", bgr[2]);
  while (true) {
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }

  return 0;
}
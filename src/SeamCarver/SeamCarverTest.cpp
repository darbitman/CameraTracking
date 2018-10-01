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
  vector<cv::Mat> bgr;
  bgr.resize(3);
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

  vector< vector<double> > outen;
  ct::SeamCarver sc1;
  sc1.energy(img, outen);

  vector< vector<char> > enint;
  enint.resize(outen.size());
  for (int i = 0; i < outen.size(); i++) {
    enint[i].resize(outen[0].size());
  }


  for (int i = 0; i < outen.size(); i++) {
    for (int j = 0; j < outen[i].size(); j++) {
      enint[i][j] = (char)(outen[i][j] * 255);
    }
  }
  cv::Mat en(enint.size(), enint[0].size(), CV_8U);
  for (int r = 0; r < enint.size(); r++) {
    for (int c = 0; c < enint[r].size(); c++) {
      en.at<char>(r, c) = enint[r][c];
    }
  }

  cv::namedWindow("energy");
  imshow("energy", en);
  
  while (true) {
    if (cv::waitKey(30) == 27) {
      std::cout << "Esc key pressed" << std::endl;
      break;
    }
  }

  return 0;
}
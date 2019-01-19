#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

using std::vector;

class DebugDisplay
{
public:
  DebugDisplay();

  template<typename _Tp>
  bool Display2DVector(const vector<vector<double>>& InputVector, _Tp NormalizationFactor);
};


inline DebugDisplay::DebugDisplay() {}

template<typename _Tp>
bool DebugDisplay::Display2DVector(const vector<vector<double>>& InputVector, _Tp NormalizationFactor)
{
  if (!(InputVector.size() > 0 && InputVector[0].size() > 0)) { return false; }

  // create an output matrix of the same dimensions as input
  cv::Mat output(InputVector.size(), InputVector[0].size(), CV_8UC1);

  for (int Row = 0; Row < InputVector.size(); Row++)
  {
    for (int Column = 0; Column < InputVector[0].size(); Column++)
    {
      output.at<uchar>(Row, Column) = (uchar)(InputVector[Row][Column] / NormalizationFactor * (2 << ((sizeof(uchar) * 8) - 1) - 1));
    }
  }

  cv::namedWindow("DebugDisplay");
  cv::imshow("DebugDisplay", output);

  while (true)
  {
    if (cv::waitKey(30) == 27)
    {
      break;
    }
  }
  return true;
}

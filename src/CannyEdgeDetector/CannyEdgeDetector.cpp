#include "CannyEdgeDetector.h"


ct::CannyEdgeDetector::CannyEdgeDetector(double lowThreshold, double maxLowThreshold, double ratio, int32_t kernelSize) {
  lowThreshold_ = lowThreshold;
  maxLowThreshold_ = maxLowThreshold;
  ratio_ = ratio;
  kernelSize_ = kernelSize;
}


bool ct::CannyEdgeDetector::RunEdgeDetector(CannyStruct& data) const {
  // can't run edge detector if the one bound to the data isn't the same one that's used
  // can't run edge detector on empty input data
  if (data.edgeDetector_ != this || data.src.empty() || data.edgeDetector_ == nullptr) {
    return false;
  }

  // local intermediate image
  cv::Mat srcGrayscale;

  // first convert picture to grayscale
  cv::cvtColor(data.src, srcGrayscale, CV_BGR2GRAY);

  // apply gaussian blur to filter noise
  cv::GaussianBlur(srcGrayscale, srcGrayscale, cv::Size(3, 3), 0);

  // Run Canny edge detector
  cv::Canny(srcGrayscale, data.detectedEdges, lowThreshold_, lowThreshold_ * ratio_, kernelSize_);

  // if you get here edge detector was successful
  return true;
}

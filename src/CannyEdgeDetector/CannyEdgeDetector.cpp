#include "CannyEdgeDetector.h"


ct::CannyEdgeDetector::CannyEdgeDetector(double lowThreshold, double highThreshold, int32_t kernelSize) {
  this->lowThreshold_ = lowThreshold;
  this->highThreshold_ = highThreshold;
  kernelSize_ = kernelSize;
}


bool ct::CannyEdgeDetector::runEdgeDetector(CannyStruct& data) const {
  // can't run edge detector on empty input data
  if (data.src.empty()) {
    return false;
  }

  // intermediate image used for grayscale and gaussian blur
  cv::Mat srcGrayscale;

  // first convert picture to grayscale
  cv::cvtColor(data.src, srcGrayscale, CV_BGR2GRAY);

  // apply gaussian blur to filter noise
  cv::GaussianBlur(srcGrayscale, srcGrayscale, cv::Size(3, 3), 0);

  // Run Canny edge detector
  // output to data.detectedEdges
  cv::Canny(srcGrayscale, data.detectedEdges, this->lowThreshold_, this->highThreshold_, this->kernelSize_);

  // if you get here edge detector was successful
  return true;
}


void ct::CannyEdgeDetector::setLowThreshold(double th) {
  this->lowThreshold_ = th;
}


void ct::CannyEdgeDetector::setHighThreshold(double th) {
  this->highThreshold_ = th;
}


void ct::CannyEdgeDetector::setKernelSize(int32_t size) {
  this->kernelSize_ = size;
}
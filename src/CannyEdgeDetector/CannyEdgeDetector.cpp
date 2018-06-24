#include "CannyEdgeDetector.h"


EdgeDetector::CannyEdgeDetector::CannyEdgeDetector(int32_t lowThreshold, int32_t maxLowThreshold, int32_t ratio, int32_t kernelSize) {
  lowThreshold_ = lowThreshold;
  maxLowThreshold_ = maxLowThreshold;
  ratio_ = ratio;
  kernelSize_ = kernelSize;
}


void EdgeDetector::CannyEdgeDetector::RunEdgeDetector(CannyStruct *data) {
  cv::Mat srcGrayscale;

  // first convert picture to grayscale
  cv::cvtColor(data->src, srcGrayscale, CV_BGR2GRAY);

  // apply gaussian blur to filter noise
  cv::GaussianBlur(srcGrayscale, data->detectedEdges, cv::Size(3, 3), 0);

  // Run Canny edge detector
  cv::Canny(data->detectedEdges, data->detectedEdges, lowThreshold_, lowThreshold_ * ratio_, kernelSize_);
  
  // set background of result to be black
  data->dst = cv::Scalar::all(0);

  // using detectedEdges as mask, copy pixels from src to dst in the locations where detectedEdges is nonzero
  data->src.copyTo(data->dst, data->detectedEdges);
}

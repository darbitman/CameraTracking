#include "CannyEdgeDetector.h"


EdgeDetector::CannyEdgeDetector::CannyEdgeDetector(int32_t lowThreshold, int32_t maxLowThreshold, int32_t ratio, int32_t kernelSize) {
  lowThreshold_ = lowThreshold;
  maxLowThreshold_ = maxLowThreshold;
  ratio_ = ratio;
  kernelSize_ = kernelSize;
}


void EdgeDetector::CannyEdgeDetector::RunEdgeDetector(CannyStruct *data) {
  cv::Mat srcGrayscale;
  cv::cvtColor(data->src, srcGrayscale, CV_BGR2GRAY);
  cv::blur(srcGrayscale, data->detectedEdges, cv::Size(3, 3));
  cv::Canny(data->detectedEdges, data->detectedEdges, lowThreshold_, lowThreshold_ * ratio_, kernelSize_);
  data->dst = cv::Scalar::all(0);
  data->src.copyTo(data->dst, data->detectedEdges);
}

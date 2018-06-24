#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>

namespace EdgeDetector {
  struct CannyStruct;

  class CannyEdgeDetector {
  public:
    CannyEdgeDetector(int32_t lowThreshold, int32_t maxLowThreshold, int32_t ratio, int32_t kernelSize = 3);
    void RunEdgeDetector(CannyStruct* data);
  private:
    int32_t lowThreshold_;
    int32_t maxLowThreshold_;
    int32_t ratio_;
    int32_t kernelSize_;

  };

  struct CannyStruct {
    CannyEdgeDetector* this_;
    cv::Mat src;
    cv::Mat dst;
    cv::Mat detectedEdges;
  };

  void CannyEdgeDetectorCallback(int value, void* userData);
}

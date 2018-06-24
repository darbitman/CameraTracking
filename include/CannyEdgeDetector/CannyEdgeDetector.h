#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>

namespace EdgeDetector {
  // forward declaration
  struct CannyStruct;

  class CannyEdgeDetector {
  public:
    // Creates an object used to run Canny edge detection
    CannyEdgeDetector(int32_t lowThreshold, int32_t maxLowThreshold, int32_t ratio, int32_t kernelSize = 3);

    // Performs Canny edge detection using data passed in through struct CannyStruct pointer
    void RunEdgeDetector(CannyStruct* data);
  private:
    int32_t lowThreshold_;
    int32_t maxLowThreshold_;
    int32_t ratio_;
    int32_t kernelSize_;

  };


  struct CannyStruct {
    CannyEdgeDetector* edgeDetector_;
    cv::Mat src;
    cv::Mat dst;
    cv::Mat detectedEdges;
  };
}

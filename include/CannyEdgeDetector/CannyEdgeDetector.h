#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>

// ct namespace (camera tracking)
namespace ct {

  // forward declaration
  struct CannyStruct;


  class CannyEdgeDetector {
  public:
    // Creates an object used to run Canny edge detection
    CannyEdgeDetector(int32_t lowThreshold, int32_t maxLowThreshold, int32_t ratio, int32_t kernelSize = 3);

    // Performs Canny edge detection using data passed in through struct CannyStruct pointer
    // returns bool to indicate if edge detector was able to run
    bool RunEdgeDetector(CannyStruct& data) const;
  private:
    int32_t lowThreshold_;
    int32_t maxLowThreshold_;
    int32_t ratio_;
    int32_t kernelSize_;

  };


  // bind a CannyEdgeDetector to input data
  struct CannyStruct {
    CannyEdgeDetector* edgeDetector_;
    cv::Mat src;
    cv::Mat dst;
    cv::Mat detectedEdges;
  };

}

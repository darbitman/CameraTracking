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
    CannyEdgeDetector(double lowThreshold, double maxLowThreshold, double ratio, int32_t kernelSize = 3);

    // Performs Canny edge detection using data passed in through struct CannyStruct pointer
    // returns bool to indicate if edge detector was able to run
    bool RunEdgeDetector(CannyStruct& data) const;
  //private:
    double lowThreshold_;
    double maxLowThreshold_;
    double ratio_;
    int32_t kernelSize_;

  };


  // bind a CannyEdgeDetector to input data
  struct CannyStruct {
    CannyEdgeDetector* edgeDetectorPtr_;
    cv::Mat src;
    cv::Mat detectedEdges;
  };

}

#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>

// ct namespace (camera tracking)
namespace ct {

  // input and output data struct
  struct CannyStruct {
    cv::Mat src;
    cv::Mat detectedEdges;
  };


  class CannyEdgeDetector {
  public:
    // Creates an object used to run Canny edge detection
    CannyEdgeDetector(double lowThreshold, double highThreshold, int32_t kernelSize = 3);

    // Performs Canny edge detection using data passed in through struct CannyStruct pointer
    // returns bool to indicate if edge detector was able to run
    bool runEdgeDetector(CannyStruct& data) const;

    // set low threshold
    void setLowThreshold(double th);

    // set high threshold
    void setHighThreshold(double th);

    // set kernel size
    void setKernelSize(int32_t size);

    CannyEdgeDetector(const CannyEdgeDetector& rhs) = delete;
    CannyEdgeDetector& operator=(const CannyEdgeDetector& rhs) = delete;
  private:
    double lowThreshold_;
    double highThreshold_;
    int32_t kernelSize_;
  };

}

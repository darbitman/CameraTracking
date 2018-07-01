#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>

// ct namespace (camera tracking)
namespace ct {
  class HOG {
  public:
    // creates a Histogram of Oriented Gradients object given standard parameters from the original paper
    // default parameters from HOG paper
    HOG(uint32_t blockSize = 16, uint32_t blockStride = 8, uint32_t cellSize = 8, uint32_t nbins = 9);

    void RunHOG(const cv::Mat& grayscaleImg, std::vector<float>& descriptors);
  private:
    uint32_t blockSize_;
    uint32_t blockStride_;
    uint32_t cellSize_;
    uint32_t nbins_;
  };
}

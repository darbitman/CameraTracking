#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>

// ct namespace (camera tracking)
namespace ct {
  class HOG {
  public:
    // creates a Histogram of Oriented Gradients object given standard parameters from the original paper
    HOG(uint32_t cellSize = 2, uint32_t cellsPerBlock = 2, uint32_t binSize = 9);
    RunHOG(const Mat& gx, const Mat& gy);
  private:
    uint32_t cellSize_;
    uint32_t cellsPerBlock_;
    uint32_t binSize_;
  };
}

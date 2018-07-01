#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>

// ct namespace (camera tracking)
namespace ct {
  class SVMTrainer {
  public:
    SVMTrainer();
    bool train(const std::vector<cv::Mat>& positiveSamples, const std::vector<cv::Mat>& negativeSamples);
  private:
  };
}

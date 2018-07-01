#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>


namespace ct {

  class Camera {
  public:
    virtual ~Camera() = 0;
    virtual bool getFrame(cv::Mat& frame) = 0;
  protected:
    cv::String location_;
  };

}

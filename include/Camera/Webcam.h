#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>

namespace ct {

  class Webcam {
  public:
    Webcam() {}
    // create new webcam
    Webcam(uint32_t index);

    Webcam(const Webcam& rhs) = delete;
    //Webcam& operator=(const Webcam& rhs) = delete;

    // cleanup resources/close stream
    ~Webcam();

    bool openStream();

    // get single frame from source
    bool getFrame(cv::Mat& frame);

  protected:
    cv::VideoCapture cap_;
    uint32_t index_;
  };

}

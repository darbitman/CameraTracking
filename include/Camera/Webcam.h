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

    // deleted copy constructor
    Webcam(const Webcam& rhs) = delete;

    // cleanup resources/close stream
    ~Webcam();

    // open stream given by Webcam::index_
    bool openStream();

    // get single frame from source
    bool getFrame(cv::Mat& outFrame);

    // assignment operator
    Webcam& operator=(Webcam& rhs);

  protected:
    cv::VideoCapture cap_;
    uint32_t index_;
  };

}

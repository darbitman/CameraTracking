#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>

namespace ct {

  class Webcam {
  public:
    Webcam() = delete;

    // create new webcam
    explicit Webcam(int32_t index);

    // deleted copy constructor
    Webcam(const Webcam& rhs) = delete;

    // move ctor
    Webcam(Webcam&& rhs);

    // cleanup resources/close stream
    ~Webcam();

    // open stream given by Webcam::index_
    bool openStream();

    // get single frame from source
    bool getFrame(cv::Mat& outFrame);

    // assignment operator
    Webcam& operator=(Webcam& rhs);

  protected:
    cv::VideoCapture *cap_;
    int32_t index_;
  };

}

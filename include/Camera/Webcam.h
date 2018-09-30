#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>

namespace ct {

  class Webcam {
  public:
    Webcam();

    // create new webcam
    explicit Webcam(int32_t index);

    // copy constructor
    Webcam(const Webcam& rhs);

    // cleanup resources/close stream
    ~Webcam();

    // open stream given by Webcam::index_
    bool openStream();

    // open stream if used default constructor
    bool openStream(uint32_t index);

    // get single frame from source
    bool getFrame(cv::Mat& outFrame);

    // assignment operator
    Webcam& operator=(const Webcam& rhs);

  protected:
    std::shared_ptr<cv::VideoCapture> cap_;
    int32_t index_;
  };

}

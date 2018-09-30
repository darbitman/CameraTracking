#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>

namespace ct {

  class IPCam {
  public:
    IPCam();

    // create new IP camera
    IPCam(cv::String location);

    // copy constructor
    IPCam(const IPCam& rhs);

    // cleanup resources/close stream
    ~IPCam();

    // open stream given by IPCam::location_
    bool openStream();

    // open stream if used a default constructor
    bool openStream(cv::String location);

    // get single frame from source
    bool getFrame(cv::Mat& outFrame);

    // delete assignment operator
    IPCam& operator=(IPCam& rhs) = delete;

  protected:
    std::shared_ptr<cv::VideoCapture> cap_;
    cv::String location_;
  };

}

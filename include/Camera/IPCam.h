#pragma once
#include "Camera.h"
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>

namespace ct {

  class IPCam : public Camera {
  public:
    // create new IP camera instance
    IPCam(cv::String location);

    // copy constructor
    IPCam(const IPCam& rhs);

    // cleanup resources/close stream
    ~IPCam();

    // open stream given by Camera::location_
    virtual bool openStream();

    // get single frame from source
    virtual bool getFrame(cv::Mat& frame);

  protected:
    cv::VideoCapture cap_;
  };

}

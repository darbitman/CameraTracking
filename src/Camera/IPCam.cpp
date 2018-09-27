#include "IPCam.h"


ct::IPCam::IPCam(cv::String location) {
  this->location_ = location;
}


ct::IPCam::IPCam(const IPCam& rhs) {
  this->location_ = rhs.location_;
  this->openStream();
}


ct::IPCam::~IPCam() {
  // if stream is open, close it/release its resources
  if (cap_.isOpened()) {
    std::cout << "Closing IPcam resources" << std::endl;
    cap_.release();
  }
}


bool ct::IPCam::openStream() {
  // if stream is close, try opening it
  if (!cap_.isOpened()) {
    cap_.open(this->location_);
  }
  if (cap_.isOpened()) {
    return true;
  }
  // failed to open stream
  else {
    std::cout << "Stream not opened" << std::endl;
    std::cin.get();
    return false;
  }
}


bool ct::IPCam::getFrame(cv::Mat& outFrame) {
  if (!cap_.isOpened()) {
    std::cout << "Stream not opened" << std::endl;
    std::cin.get();
    return false;
  }
  return cap_.read(outFrame);
}

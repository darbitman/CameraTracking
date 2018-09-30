#include "IPCam.h"


ct::IPCam::IPCam() {
  this->cap_ = std::make_shared<cv::VideoCapture>();
}


ct::IPCam::IPCam(cv::String location) {
  this->location_ = location;
  this->cap_ = std::make_shared<cv::VideoCapture>();
}


ct::IPCam::IPCam(const IPCam& rhs) {
  this->cap_ = rhs.cap_;
}


ct::IPCam::~IPCam() {
  // if only one reference to stream, then its resources can be released
  if (this->cap_.get() != nullptr && this->cap_.use_count() == 1) {
    std::cout << "Closing IPcam resources" << std::endl;
    this->cap_->release();
  }
}


bool ct::IPCam::openStream() {
  if (this->cap_.get() == nullptr || this->location_.size() == 0) {
    return false;
  }

  // if stream is closed, try opening it
  if (!this->cap_->isOpened()) {
    this->cap_->open(this->location_);
  }

  // check if managed to open
  if (cap_->isOpened()) {
    return true;
  }
  else {
    std::cout << "Stream not opened" << std::endl;
    std::cin.get();
    return false;
  }
}

bool ct::IPCam::openStream(cv::String location) {
  this->location_ = location;
  return this->openStream();
}


bool ct::IPCam::getFrame(cv::Mat& outFrame) {
  if (!cap_->isOpened()) {
    std::cout << "Stream not opened" << std::endl;
    std::cin.get();
    return false;
  }
  return cap_->read(outFrame);
}

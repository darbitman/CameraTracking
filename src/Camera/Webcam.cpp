#include "Webcam.h"


ct::Webcam::Webcam(int32_t index): index_(index) {
  cap_ = new cv::VideoCapture();
}


ct::Webcam::Webcam(Webcam&& rhs) {
  this->cap_ = rhs.cap_;
  rhs.cap_ = nullptr;
  this->index_ = rhs.index_;
}


ct::Webcam::~Webcam() {
  // if stream is open, close it/release its resources
  if (this->cap_ && this->cap_->isOpened()) {
    std::cout << "Closing webcam resources" << std::endl;
    this->cap_->release();
  }
  delete this->cap_;
}


bool ct::Webcam::openStream() {
  if (this->cap_ == nullptr) {
    return false;
  }
  // if stream is close, try opening it
  if (!this->cap_->isOpened()) {
    this->cap_->open(this->index_);
  }
  if (this->cap_->isOpened()) {
    return true;
  }
  // failed to open stream
  else {
    std::cout << "Stream not opened" << std::endl;
    std::cin.get();
    return false;
  }
}


bool ct::Webcam::getFrame(cv::Mat& outFrame) {
  if (!cap_->isOpened()) {
    std::cout << "Stream not opened" << std::endl;
    std::cin.get();
    return false;
  }
  return this->cap_->read(outFrame);
}


ct::Webcam& ct::Webcam::operator=(Webcam& rhs) {
  // need to open stream before performaning a copy of cv::VideoCapture
  rhs.openStream();
  this->cap_ = rhs.cap_;
  this->index_ = rhs.index_;
  if (!this->cap_->isOpened()) {
    this->cap_->open(this->index_);
  }
  return *this;
}

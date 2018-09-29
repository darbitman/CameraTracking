#include "Webcam.h"


ct::Webcam::Webcam() {
  this->index_ = -1;
};


ct::Webcam::Webcam(int32_t index): index_(index) {
  cap_ = std::make_shared<cv::VideoCapture>();
}


ct::Webcam::Webcam(const Webcam& rhs) {
  this->index_ = rhs.index_;
  this->cap_ = rhs.cap_;
}


ct::Webcam::~Webcam() {
  // if only one reference to stream, then its resources can be released
  if (this->cap_.get() != nullptr && this->cap_.use_count() == 1) {
    this->cap_->release();
  }
}


bool ct::Webcam::openStream() {
  if (this->cap_.get() == nullptr || this->index_ == -1) {
    return false;
  }

  // if stream is closed, try opening it
  if (!this->cap_->isOpened()) {
    this->cap_->open(this->index_);
  }

  // check if managed to open
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


ct::Webcam& ct::Webcam::operator=(const Webcam& rhs) {
  this->index_ = rhs.index_;
  this->cap_ = rhs.cap_;
  return *this;
}

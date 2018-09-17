#include "Webcam.h"


ct::Webcam::Webcam(uint32_t index) {
  this->index_ = index;
}


ct::Webcam::~Webcam() {
  // if stream is open, close it/release its resources
  if (cap_.isOpened()) {
    cap_.release();
  }
}


bool ct::Webcam::openStream() {
  // if stream is close, try opening it
  if (!cap_.isOpened()) {
    cap_.open(this->index_);
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


bool ct::Webcam::getFrame(cv::Mat& frame) {
  if (!cap_.isOpened()) {
    std::cout << "Stream not opened" << std::endl;
    std::cin.get();
    return false;
  }
  return cap_.read(frame);
}

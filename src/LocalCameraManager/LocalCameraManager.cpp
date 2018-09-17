#include "LocalCameraManager.h"

ct::LocalCameraManager::LocalCameraManager() {
  this->count_ = 0;
  // search for all local cameras and register them with application
  uint32_t index = 0;
  while (true) {
    cv::VideoCapture c;
    // try opening stream and check if stream isn't already mapped
    if (c.open(index) && IndexToCamMap_.count(index) == 0) {
      IndexToCamMap_[index] = Webcam(index);
      index++;
      this->count_++;
    }
    else {
      break;
    }
  }
}


ct::LocalCameraManager::~LocalCameraManager() {}


bool ct::LocalCameraManager::DeleteCamera(uint32_t index) {
  if (IndexToCamMap_.count(index) > 0) {
    IndexToCamMap_.erase(index);
    return true;
  }
  else {
    return false;
  }
}


uint32_t ct::LocalCameraManager::getCameraCount() const {
  return count_;
}
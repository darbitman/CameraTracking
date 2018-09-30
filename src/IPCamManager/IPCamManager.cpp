#include "IPCamManager.h"
#include <iostream>


ct::IPCamManager::IPCamManager() {
  this->cameraCount_ = 0;
  this->nextCameraIterator_ = 0;
  this->nextCameraIndex_ = 0;
}


ct::IPCamManager::~IPCamManager() {}


bool ct::IPCamManager::addCamera(cv::String location) {
  IPCam c(location);
  if (c.openStream()) {
    this->indexToCamMap_[this->nextCameraIndex_++] = c;
    this->cameraCount_++;
  }
}


bool ct::IPCamManager::deleteCamera(uint32_t index) {
  if (this->indexToCamMap_.count(index) > 0) {
    this->indexToCamMap_.erase(index);
    this->cameraCount_--;
    return true;
  }
  else {
    return false;
  }
}


uint32_t ct::IPCamManager::getCameraCount() const {
  return this->cameraCount_;
}


bool ct::IPCamManager::getCameraAtIndex(uint32_t index, Webcam& outCamRef) {
  // no cameras are managed
  if (this->cameraCount_ == 0) {
    return false;
  }
  // camera at index is not managed
  if (this->indexToCamMap_.count(index) == 0) {
    return false;
  }
  outCamRef = this->indexToCamMap_[index];
  return true;
}



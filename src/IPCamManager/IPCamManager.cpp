#include "IPCamManager.h"
#include <iostream>


ct::IPCamManager::IPCamManager() {
  this->cameraCount_ = 0;
  this->nextCameraIndex_ = 0;
}


ct::IPCamManager::~IPCamManager() {}


bool ct::IPCamManager::addCamera(cv::String location) {
  IPCam c(location);
  if (c.openStream()) {
    this->indexToCamMap_[this->nextCameraIndex_++] = c;
    this->cameraCount_++;
    if (this->cameraCount_ == 1) {
      this->camIter_ = this->indexToCamMap_.begin();
    }
    return true;
  }
  else {
    return false;
  }
}


bool ct::IPCamManager::deleteCamera(uint32_t index) {
  if (this->indexToCamMap_.count(index) > 0) {
    // if next camera to return via iterator is deleted, go to the next camera
    if (this->camIter_->first == index) {
      this->camIter_++;
      // if past the last element, wrap around to front
      if (this->camIter_ == this->indexToCamMap_.end()) {
        this->camIter_ = this->indexToCamMap_.begin();
      }
    }
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


bool ct::IPCamManager::getCameraAtIndex(uint32_t index, ct::IPCam& outCamRef) {
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


bool ct::IPCamManager::getNextCamera(ct::IPCam& outCamRef) {
  if (this->cameraCount_ > 0) {
    outCamRef = this->camIter_->second;
    this->camIter_++;
    // if past the last element, wrap around to front
    if (this->camIter_ == this->indexToCamMap_.end()) {
      this->camIter_ = this->indexToCamMap_.begin();
    }
    return true;
  }
  return false;
}
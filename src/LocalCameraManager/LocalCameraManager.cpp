#include "LocalCameraManager.h"
#include <iostream>


ct::LocalCameraManager::LocalCameraManager() {
  this->cameraCount_ = 0;
  this->nextCameraIterator_ = 0;
  // search for local cameras and register them with application
  uint32_t index = 0;
  while (true) {
    cv::VideoCapture c;
    // try opening stream and check if stream isn't already mapped
    if (c.open(index) && this->indexToCamMap_.count(index) == 0) {
      // free resources
      c.release();
      this->indexToCamMap_[index] = Webcam(index);
      index++;
      this->cameraCount_++;
      // if first camera added, initialize iterator
      if (this->cameraCount_ == 1) {
        this->camIter_ = this->indexToCamMap_.begin();
      }
    }
    else {
      break;
    }
  }
}



ct::LocalCameraManager::~LocalCameraManager() {}


bool ct::LocalCameraManager::addCamera(uint32_t index) {
  // Webcam already registered
  if (this->indexToCamMap_.count(index) > 0) {
    return false;
  }
  cv::VideoCapture c;
  // try opening stream and check if stream isn't already mapped
  if (c.open(index)) {
    // free resources
    c.release();
    this->indexToCamMap_[index] = Webcam(index);
    this->cameraCount_++;
    // adding first camera manually, initialize iterator
    if (this->cameraCount_ == 1) {
      this->camIter_ = this->indexToCamMap_.begin();
    }
    return true;
  }
  else {
    c.release();
    return false;
  }
}

bool ct::LocalCameraManager::deleteCamera(uint32_t index) {
  if (this->indexToCamMap_.count(index) > 0) {
    // if iterator is pointing to the camera to be deleted, point to the next cam
    if (this->camIter_->first == index) {
      this->camIter_++;
      // if past the last element, wrap around to front
      if (this->camIter_ == this->indexToCamMap_.end() && this->cameraCount_ > 1) {
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


uint32_t ct::LocalCameraManager::getCameraCount() const {
  return cameraCount_;
}


bool ct::LocalCameraManager::getNextCameraIndex(uint32_t& outIndex) {
  if (this->cameraCount_ == 0) {
    return false;
  }
  else {
    outIndex = this->nextCameraIterator_;
    this->nextCameraIterator_ = (this->nextCameraIterator_ + 1) % this->cameraCount_;
  }
  return true;
}


bool ct::LocalCameraManager::getCameraAtIndex(uint32_t index, Webcam& outCamRef) {
  // no cameras are managed
  if (this->cameraCount_ == 0) {
    return false;
  }
  // camera at index is not managed
  if (this->indexToCamMap_.count(index) == 0) {
    return false;
  }
  outCamRef = indexToCamMap_[index];
  return true;
}


bool ct::LocalCameraManager::getNextCamera(Webcam& outCamRef) {
  //uint32_t index = 0;
  //if (getNextCameraIndex(index)) {
  //  return getCameraAtIndex(index, outCamRef);
  //}
  //else {
  //  return false;
  //}
  // TODO use iterator
  if (this->cameraCount_ > 0) {
    outCamRef = this->camIter_->second;
    this->camIter_++;
    // if past the last element, wrap around to front
    if (this->camIter_ == this->indexToCamMap_.end()) {
      this->camIter_ = this->indexToCamMap_.begin();
    }
  }
  else {
    return false;
  }
}

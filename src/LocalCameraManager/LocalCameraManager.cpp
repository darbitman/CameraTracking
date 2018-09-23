#include "LocalCameraManager.h"


ct::LocalCameraManager::LocalCameraManager() {
  this->cameraCount_ = 0;
  this->nextCameraIterator_ = 0;
  // search for all local cameras and register them with application
  uint32_t index = 0;
  while (true) {
    cv::VideoCapture c;
    // try opening stream and check if stream isn't already mapped
    if (c.open(index) && indexToCamMap_.count(index) == 0) {
      // free resources 
      c.release();
      indexToCamMap_[index] = Webcam(index);
      index++;
      this->cameraCount_++;
    }
    else {
      break;
    }
  }
}


ct::LocalCameraManager::~LocalCameraManager() {}


bool ct::LocalCameraManager::DeleteCamera(uint32_t index) {
  if (indexToCamMap_.count(index) > 0) {
    indexToCamMap_.erase(index);
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


bool ct::LocalCameraManager::getNextCameraIndex(uint32_t& index) {
  if (this->cameraCount_ == 0) {
    return false;
  }
  else {
    index = this->nextCameraIterator_;
    this->nextCameraIterator_ = (this->nextCameraIterator_ + 1) % this->cameraCount_;
  }
  return true;
}


bool ct::LocalCameraManager::getCameraAtIndex(uint32_t index, Webcam*& camPtr) {
  // no cameras are managed
  if (this->cameraCount_ == 0) {
    return false;
  }
  // camera at index is not managed
  if (this->indexToCamMap_.count(index) == 0) {
    return false;
  }
  camPtr = &indexToCamMap_[index];
  return true;
}


bool ct::LocalCameraManager::getNextCamera(Webcam*& camPtr) {
  uint32_t index = 0;
  if (getNextCameraIndex(index)) {
    return getCameraAtIndex(index, camPtr);
  }
  else {
    return false;
  }
}

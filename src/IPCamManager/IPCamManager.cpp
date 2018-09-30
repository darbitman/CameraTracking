#include "IPCamManager.h"
#include <iostream>


ct::IPCamManager::IPCamManager() {
  this->cameraCount_ = 0;
  this->nextCameraIterator_ = 0;
}

bool ct::IPCamManager::addCamera(cv::String location) {
  IPCam c(location);
  if (c.openStream()) {
    this->indexToCamMap_[this->cameraCount_++] = c;
  }
}
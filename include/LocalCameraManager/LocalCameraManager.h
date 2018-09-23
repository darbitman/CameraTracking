#pragma once
#include "Webcam.h"
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>


using std::map;

namespace ct {
  class LocalCameraManager {
  public:
    LocalCameraManager();
    ~LocalCameraManager();

    // delete a local camera from the manager and returns true if successful
    bool DeleteCamera(uint32_t index);

    // return number of managed cameras
    uint32_t getCameraCount() const;

    // iterates through cameras
    bool getNextCameraIndex(uint32_t& index);

    // get camera at index
    bool getCameraAtIndex(uint32_t index, Webcam*& camPtr);

    LocalCameraManager(const LocalCameraManager& rhs) = delete;
    LocalCameraManager& operator=(const LocalCameraManager& rhs) = delete;
  private:
    map<uint32_t, Webcam> indexToCamMap_;
    uint32_t cameraCount_;
    uint32_t nextCameraIterator_;
  };

}

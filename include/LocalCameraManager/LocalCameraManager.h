#pragma once
#include "Webcam.h"
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>
#include <list>

using std::map;

namespace ct {
  class LocalCameraManager {
  public:
    LocalCameraManager();
    ~LocalCameraManager();
    bool DeleteCamera(uint32_t index);
    uint32_t getCameraCount() const;

    LocalCameraManager(const LocalCameraManager& rhs) = delete;
    LocalCameraManager& operator=(const LocalCameraManager& rhs) = delete;
  private:
    map<uint32_t, Webcam> IndexToCamMap_;
    uint32_t count_;
  };

}

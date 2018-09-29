#pragma once
#include "IPCam.h"
#include <opencv2/opencv.hpp>
#include <stdint.h>


using std::map;


namespace ct {

  class IPCamManager {
    IPCamManager();
    ~IPCamManager();

    // delete a local camera from the manager and return true if successful
    bool deleteCamera(uint32_t index);

    // return number of managed cameras
    uint32_t getCameraCount() const;

    // iterates through cameras
    // returns true if operation completed successfully
    // index returned through return parameter
    bool getNextCameraIndex(uint32_t& outIndex);

    // get camera at index
    // returns true if operation completed successfully
    // pointer to an IPCam is returned through return paramter
    // pointer is passed by reference so client must pass in a pointer
  };

}

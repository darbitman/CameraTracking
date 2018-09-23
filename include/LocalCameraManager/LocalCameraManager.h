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
    // returns true if operation completed successfully
    // index returned through return parameter
    bool getNextCameraIndex(uint32_t& index);

    // get camera at index
    // returns true if operation completed sucessfully
    // pointer to a Webcam object is returned through return parameter
    // pointer is passed by reference so client must pass in a pointer
    bool getCameraAtIndex(uint32_t index, Webcam*& camPtr);

    // get next camera
    // returns true if operation completed sucessfully
    // pointer to a Webcam object is returned through return parameter
    // pointer is passed by reference so client must pass in a pointer
    bool getNextCamera(Webcam*& camPtr);

    LocalCameraManager(const LocalCameraManager& rhs) = delete;
    LocalCameraManager& operator=(const LocalCameraManager& rhs) = delete;
  private:
    map<uint32_t, Webcam> indexToCamMap_;
    uint32_t cameraCount_;
    uint32_t nextCameraIterator_;
  };

}

#pragma once
#include "IPCam.h"
#include <opencv2/opencv.hpp>
#include <stdint.h>


using std::map;


namespace ct {

  class IPCamManager {
  public:
    IPCamManager();
    ~IPCamManager();

    // add new IPCam
    bool addCamera(cv::String location);

    // delete an IPCam from the manager and return true if successful
    bool deleteCamera(uint32_t index);

    // return number of managed cameras
    uint32_t getCameraCount() const;

    // get camera at index
    // returns true if operation completed successfully
    // pointer to an IPCam is returned through return paramter
    // pointer is passed by reference so client must pass in a pointer
    bool getCameraAtIndex(uint32_t index, IPCam& outCamRef);

    // get next camera
    // returns true if operation completed successfully
    // pointer to a IPCam object is returned through return parameter
    // pointer is passed by reference so client must pass in a pointer
    bool getNextCamera(IPCam& outCamRef);
  protected:
    map<uint32_t, IPCam>::iterator camIter_;
    map<uint32_t, IPCam> indexToCamMap_;
    uint32_t cameraCount_;
    uint32_t nextCameraIndex_;
  };

}

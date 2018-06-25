include_directories("../../include/Camera")

add_library(Camera "")
target_sources(Camera PRIVATE
               "Camera.cpp"
               "../../include/Camera/Camera.h")

               
add_library(IPCam "")
target_sources(IPCam PRIVATE
               "IPCam.cpp"
               "../../include/Camera/IPCam.h")
find_package(OpenCV REQUIRED)
include_directories("../../include/IPCamManager"
                    "../../include/Camera")
add_library(IPCamManager "")
target_sources(IPCamManager PRIVATE
               "IPCamManager.cpp"
               "../../include/IPCamManager/IPCamManager.h")
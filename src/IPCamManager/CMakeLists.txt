find_package(OpenCV REQUIRED)
include_directories("../../include/IPCamManager"
                    "../../include/Camera")
add_library(IPCamManager "")
target_sources(IPCamManager PRIVATE
               "IPCamManager.cpp"
               "../../include/IPCamManager/IPCamManager.h")

add_executable(IPCamManagerTest
               IPCamManagerTest.cpp)
target_link_libraries(IPCamManagerTest
                      IPCamManager
                      IPCam
                      ${OpenCV_LIBS})
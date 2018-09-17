find_package(OpenCV REQUIRED)
include_directories("../../include/Camera")

add_library(IPCam "")
target_sources(IPCam PRIVATE
               "IPCam.cpp"
               "../../include/Camera/IPCam.h")

add_library(Webcam "")
target_sources(Webcam PRIVATE
               "Webcam.cpp"
               "../../include/Camera/Webcam.h")
                      
add_executable(IPCamTest
               IPCamTest.cpp)
target_link_libraries(IPCamTest
                      IPCam
                      ${OpenCV_LIBS})

add_executable(WebcamTest
               WebcamTest.cpp)
target_link_libraries(WebcamTest
                      Webcam
                      ${OpenCV_LIBS})
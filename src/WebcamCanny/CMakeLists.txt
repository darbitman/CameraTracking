find_package(OpenCV REQUIRED)
include_directories("../../include/Camera"
                    "../../include/LocalCameraManager"
                    "../../include/CannyEdgeDetector")

add_executable(WebcamCannyTest
               WebcamCannyTest.cpp)
target_link_libraries(WebcamCannyTest
                      Webcam
                      LocalCameraManager
                      CannyEdgeDetector
                      ${OpenCV_LIBS})
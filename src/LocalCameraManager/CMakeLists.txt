find_package(OpenCV REQUIRED)
include_directories("../../include/LocalCameraManager" "../../include/Camera")
add_library(LocalCameraManager "")
target_sources(LocalCameraManager PRIVATE
               "LocalCameraManager.cpp"
               "../../include/LocalCameraManager/LocalCameraManager.h"
               "../Camera/Webcam.cpp"
               "../../include/Camera/Webcam.h")

add_executable(LocalCameraManagerTest
               LocalCameraManagerTest.cpp)
target_link_libraries(LocalCameraManagerTest
                      LocalCameraManager
                      ${OpenCV_LIBS})
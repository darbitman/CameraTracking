find_package(OpenCV REQUIRED)
include_directories("../../include/LocalCameras")
add_library(LocalCameras "")
target_sources(LocalCameras PRIVATE
               "LocalCameras.cpp"
               "../../include/LocalCameras/LocalCameras.h")
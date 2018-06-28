find_package(OpenCV REQUIRED)
add_library(CannyEdgeDetector "")
include_directories("../../include/CannyEdgeDetector")
target_sources(CannyEdgeDetector PRIVATE
               "CannyEdgeDetector.cpp"
               "../../include/CannyEdgeDetector/CannyEdgeDetector.h")
add_executable(CannyEdgeDetectorTest
               CannyEdgeDetectorTest.cpp)
target_link_libraries(CannyEdgeDetectorTest
                      CannyEdgeDetector
                      ${OpenCV_LIBS})
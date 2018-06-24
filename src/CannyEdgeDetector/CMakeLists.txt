add_library(CannyEdgeDetector "")
include_directories("../../include/CannyEdgeDetector")
target_sources(CannyEdgeDetector PRIVATE
               "CannyEdgeDetector.cpp"
               "../../include/CannyEdgeDetector/CannyEdgeDetector.h")

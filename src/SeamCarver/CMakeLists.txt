find_package(OpenCV REQUIRED)
include_directories("../../include/SeamCarver"
                    "../../include/ResizablePriorityQueue")
add_library(SeamCarver "")
target_sources(SeamCarver PRIVATE
               "SeamCarver.cpp"
               "../../include/SeamCarver/SeamCarver.h"
               "../../include/ResizablePriorityQueue/ConstSizeMinBinaryHeap.h")

add_executable(SeamCarverTest
               SeamCarverTest.cpp)
target_link_libraries(SeamCarverTest
                      SeamCarver
                      SeamCarverKeepout
                      ${OpenCV_LIBS})
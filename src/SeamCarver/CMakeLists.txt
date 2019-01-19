include_directories("../../include/SeamCarver"
                    "../../include/ResizablePriorityQueue")
                    
add_library(SeamCarver "")
target_sources(SeamCarver PRIVATE
               "SeamCarver.cpp"
               "../../include/SeamCarver/SeamCarver.h"
               "../../include/ResizablePriorityQueue/ConstSizeMinBinaryHeap.h")
               
add_library(SeamCarverKeepout "")
target_sources(SeamCarverKeepout PRIVATE
               "SeamCarverKeepout.cpp"
               "../../include/SeamCarver/SeamCarverKeepout.h")
               
add_library(PixelEnergy2D
            STATIC
            "")
target_sources(PixelEnergy2D
               PRIVATE
               "PixelEnergy2D.cpp"
               "../../include/SeamCarver/PixelEnergy2D.h")

add_executable(PixelEnergy2DTest
               PixelEnergy2DTest.cpp)
target_link_libraries(PixelEnergy2DTest
                      PixelEnergy2D
                      ${OpenCV_LIBS}
                      gtest_main)

add_executable(SeamCarverTest
               SeamCarverTest.cpp)
target_link_libraries(SeamCarverTest
                      SeamCarver
                      SeamCarverKeepout
                      ${OpenCV_LIBS}
                      gtest_main
                      PixelEnergy2D)
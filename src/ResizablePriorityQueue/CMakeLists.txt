include_directories("../../include/ResizablePriorityQueue")

add_executable(ResizablePriorityQueueTest
               ResizablePriorityQueueTest.cpp
               "../../include/ResizablePriorityQueue/ResizablePriorityQueue.h"
               "../../include/ResizablePriorityQueue/ConstSizeMinBinaryHeap.h")
target_link_libraries(ResizablePriorityQueueTest
                      gtest_main)
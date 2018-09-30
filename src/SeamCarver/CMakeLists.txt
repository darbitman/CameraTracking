find_package(OpenCV REQUIRED)
include_directories("../../include/SeamCarver")
add_library(SeamCarver "")
target_sources(SeamCarver PRIVATE
               "SeamCarver.cpp"
               "../../include/SeamCarver/SeamCarver.h")

add_executable(SeamCarverTest
               SeamCarverTest.cpp)
target_link_libraries(SeamCarverTest
                      SeamCarver
                      ${OpenCV_LIBS})
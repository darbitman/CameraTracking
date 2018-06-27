#include "HOG.h"


ct::HOG::HOG(uint32_t cellSize, uint32_t cellsPerBlock, uint32_t binSize) {

}


void ct::HOG::RunHOG(const cv::Mat& grayscaleImg, cv::Mat& descriptors) {
  cv::HOGDescriptor hog;
}
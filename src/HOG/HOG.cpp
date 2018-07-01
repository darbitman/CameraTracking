#include "HOG.h"


ct::HOG::HOG(uint32_t blockSize, uint32_t blockStride, uint32_t cellSize, uint32_t nbins) {
  this->blockSize_ = blockSize;
  this->blockStride_ = blockStride;
  this->cellSize_ = cellSize;
  this->nbins_ = nbins;
}


void ct::HOG::RunHOG(const cv::Mat& grayscaleImg, std::vector<float>& descriptors) {
  cv::Size winSize(grayscaleImg.cols, grayscaleImg.rows);
  cv::HOGDescriptor hog(winSize,
    cv::Size(this->blockSize_, this->blockSize_),
    cv::Size(this->blockStride_, this->blockStride_),
    cv::Size(this->cellSize_, this->cellSize_),
    this->nbins_);

  hog.compute(grayscaleImg, descriptors);
}
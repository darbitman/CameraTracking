#include "SeamCarverKeepout.h"

bool ct::SeamCarverKeepout::findAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn) {
  // check to make sure keepout region doesn't extend beyond borders of image
  if (this->keepoutRegionExists_) {
    if (this->keepoutRegion_.col_ < 0 || this->keepoutRegion_.col_ >= img.size().width ||
        this->keepoutRegion_.row_ < 0 || this->keepoutRegion_.row_ >= img.size().height) {
      throw std::out_of_range("keepout region starting column and/or row is beyond the borders");
    }
    if (this->keepoutRegion_.col_ + this->keepoutRegion_.width_ < 0 || this->keepoutRegion_.col_ + this->keepoutRegion_.width_ >= img.size().width ||
        this->keepoutRegion_.row_ + this->keepoutRegion_.height_ < 0 || this->keepoutRegion_.row_ + this->keepoutRegion_.height_ >= img.size().width) {
      throw std::out_of_range("keepout region range extends beyond the borders");
    }
  }

  // set pixels to avoid by setting them as previously marked
  if (this->keepoutRegionExists_) {
    if (marked.size() != img.size().height ) {
      marked.resize(img.size().height);
      for (int32_t r = 0; r < img.size().height; r++) {
        marked[r].resize(img.size().width);
        for (int32_t c = 0; c < img.size().width; c++) {
          marked[r][c] = false;
        }
      }
    }

    for (int32_t r = this->keepoutRegion_.row_; r < this->keepoutRegion_.row_ + this->keepoutRegion_.height_; r++) {
      for (int32_t c = this->keepoutRegion_.col_; c < this->keepoutRegion_.col_ + this->keepoutRegion_.width_; c++) {
        marked[r][c] = true;
      }
    }
  }

  return SeamCarver::findAndRemoveVerticalSeams(numSeams, img, outImg, computeEnergyFn);
}


void ct::SeamCarverKeepout::setKeepoutRegion(int32_t row, int32_t col, int32_t width, int32_t height) {
  this->keepoutRegion_.row_ = row;
  this->keepoutRegion_.col_ = col;
  this->keepoutRegion_.width_ = width;
  this->keepoutRegion_.height_ = height;
  this->keepoutRegionExists_ = true;
}


void ct::SeamCarverKeepout::deleteKeepoutRegion() {
  if (this->keepoutRegionExists_) {
    if (this->keepoutRegion_.row_ + this->keepoutRegion_.height_ >= marked.size()) {

    }

    this->keepoutRegionExists_ = false;
  }

  

}
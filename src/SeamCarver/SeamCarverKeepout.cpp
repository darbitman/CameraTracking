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

  // if marked matrix hasn't been initialized, need to initialize so that keepout region data isn't overwritten in a future memory initialization
  // default initialization to false
  // then initialize keepout region
  if (this->keepoutRegionExists_) {
    if (MarkedPixels.size() != img.size().height ) {
      MarkedPixels.resize(img.size().height);
      for (int32_t r = 0; r < img.size().height; r++) {
        MarkedPixels[r].resize(img.size().width);
        for (int32_t c = 0; c < img.size().width; c++) {
          MarkedPixels[r][c] = false;
        }
      }
    }

    // set pixels to avoid by setting them as previously marked
    for (int32_t r = this->keepoutRegion_.row_; r < this->keepoutRegion_.row_ + this->keepoutRegion_.height_; r++) {
      for (int32_t c = this->keepoutRegion_.col_; c < this->keepoutRegion_.col_ + this->keepoutRegion_.width_; c++) {
        MarkedPixels[r][c] = true;
      }
    }
  }

  return KSeamCarver::FindAndRemoveVerticalSeams(numSeams, img, outImg, computeEnergyFn);
}


void ct::SeamCarverKeepout::setKeepoutRegion(int32_t row, int32_t col, int32_t height, int32_t width) {
  this->keepoutRegion_.row_ = row;
  this->keepoutRegion_.col_ = col;
  this->keepoutRegion_.height_ = height;
  this->keepoutRegion_.width_ = width;
  this->keepoutRegionExists_ = true;
}


void ct::SeamCarverKeepout::deleteKeepoutRegion() {
  // check if marked matrix has been allocated
  // if not, then don't need to unmark pixels in the keepout region
  if (this->keepoutRegionExists_ && this->MarkedPixels.size() > 0) {
    // unmark mixels marked by keepout region
    if (this->keepoutRegion_.row_ + this->keepoutRegion_.height_ >= (int32_t)MarkedPixels.size()) {
      for (int32_t r = this->keepoutRegion_.row_; r < this->keepoutRegion_.row_ + this->keepoutRegion_.height_; r++) {
        for (int32_t c = this->keepoutRegion_.col_; c < this->keepoutRegion_.col_ + this->keepoutRegion_.width_; c++) {
          MarkedPixels[r][c] = false;
        }
      }
    }
  }
  this->keepoutRegionExists_ = false;

}
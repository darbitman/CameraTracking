#include "SeamCarver.h"


ct::SeamCarver::SeamCarver(double margin_energy) : MARGIN_ENERGY(margin_energy) {
}


ct::SeamCarver::~SeamCarver() {
}


bool ct::SeamCarver::removeVerticalSeams(int32_t numSeams, cv::Mat& img, cv::Mat& outImg) {
  return false;
}


bool ct::SeamCarver::removeHorizontalSeams(int32_t numSeams, cv::Mat& img, cv::Mat& outImg) {
  return false;
}



bool ct::SeamCarver::energyAt(cv::Mat& pixels, int32_t r, int32_t c, double& outEnergy) {
  // if r or c out of bounds, then return false
  if (r < 0 || c < 0 || r > pixels.size().height - 1 || c > pixels.size().width - 1) {
    return false;
  }
  // return energy for border pixels
  if (r == 0 || c == 0 || r == pixels.size().height - 1 || c == pixels.size().width - 1) {
    outEnergy = this->MARGIN_ENERGY;
    return true;
  }
  else {

    return true;
  }
}


bool ct::SeamCarver::energy(cv::Mat& pixels, cv::Mat& outPixelEnergy) {
  return false;
}


bool ct::SeamCarver::removeVerticalSeam(cv::Mat& img, cv::Mat& outImg, vector<int>& seam) {
  return false;
}


bool ct::SeamCarver::removeHorizontalSeam(cv::Mat& img, cv::Mat& outImg, vector<int>& seam) {
  return false;
}
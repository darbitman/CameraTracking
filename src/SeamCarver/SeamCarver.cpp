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



bool ct::SeamCarver::energyAt(const vector<cv::Mat>& bgr, int32_t r, int32_t c, double& outEnergy) {
  // if r or c out of bounds, then return false
  if (r < 0 || c < 0 || r > bgr[0].size().height - 1 || c > bgr[0].size().width - 1) {
    return false;
  }
  // return energy for border pixels
  if (r == 0 || c == 0 || r == bgr[0].size().height - 1 || c == bgr[0].size().width - 1) {
    outEnergy = this->MARGIN_ENERGY;
  }
  else {
    // compute energy for every pixel by computing gradient of colors
    // DeltaX = DeltaRx^2 + DeltaGx^2 + DeltaBx^2
    // DeltaY = DeltaRy^2 + DeltaGy^2 + DeltaBy^2
    // energy = sqrt(DeltaX + DeltaY)
    double deltaSquareX = (pow(bgr[2].at<char>(r, c + 1) - bgr[2].at<char>(r, c - 1), 2.0) +
                           pow(bgr[1].at<char>(r, c + 1) - bgr[1].at<char>(r, c - 1), 2.0) +
                           pow(bgr[0].at<char>(r, c + 1) - bgr[0].at<char>(r, c - 1), 2.0));
    double deltaSquareY = (pow(bgr[2].at<char>(r + 1, c) - bgr[2].at<char>(r - 1, c), 2.0) +
                           pow(bgr[1].at<char>(r + 1, c) - bgr[1].at<char>(r - 1, c), 2.0) +
                           pow(bgr[0].at<char>(r + 1, c) - bgr[0].at<char>(r - 1, c), 2.0));
    outEnergy = sqrt(deltaSquareX + deltaSquareY) / 624.6198844097; // normalize
  }
  return true;
}


bool ct::SeamCarver::energy(const cv::Mat& pixels, vector< vector<double> >& outPixelEnergy) {
  // split input pixels into separate blue, green, red channels
  vector<cv::Mat> bgr;
  bgr.resize(3);
  cv::split(pixels, bgr);

  // resize output if necessary
  if (outPixelEnergy.size() != pixels.size().height) {
    outPixelEnergy.resize(pixels.size().height);
  }
  if (outPixelEnergy[0].size() != pixels.size().width) {
    for (int i = 0; i < pixels.size().height; i++) {
      outPixelEnergy[i].resize(pixels.size().width);
    }
  }
  double computedEnergy = 0.0;
  for (int r = 0; r < pixels.size().height; r++) {
    for (int c = 0; c < pixels.size().width; c++) {
      if (this->energyAt(bgr, r, c, computedEnergy)) {
      }
      outPixelEnergy[r][c] = computedEnergy;
    }
  }
  return false;
}


void ct::SeamCarver::findVerticalSeam(vector< vector<double> >& pixelEnergy, vector<int>& outSeam) {
  // resize output seam to number of rows i.e. height
  outSeam.resize(pixelEnergy.size());

  // array will store cumulative energy to each pixel
  vector< vector<double> > totalEnergyTo;
  totalEnergyTo.resize(pixelEnergy.size());
  for (int r = 0; r < pixelEnergy.size(); i++) {
    totalEnergyTo[r].resize(pixelEnergy[r].size());
  }

  // initialize first row
  for (int c = 0; c < totalEnergyTo[0].size(); c++) {
    totalEnergyTo[0][c] = pixelEnergy[0][c];
    // TODO store cumulative path in pixelTo vector
  }
}


bool ct::SeamCarver::removeVerticalSeam(cv::Mat& img, cv::Mat& outImg, vector<int>& seam) {
  return false;
}


bool ct::SeamCarver::removeHorizontalSeam(cv::Mat& img, cv::Mat& outImg, vector<int>& seam) {
  return false;
}
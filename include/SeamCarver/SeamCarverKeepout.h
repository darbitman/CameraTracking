#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include "SeamCarver.h"


namespace ct {

  struct KeepoutRegionStruct {
    int32_t row_;
    int32_t col_;
    int32_t height_;
    int32_t width_;

    KeepoutRegionStruct(int32_t row, int32_t col, int32_t height, int32_t width) :
      row_(row), col_(col), height_(height), width_(width) {}

    KeepoutRegionStruct() : row_(0), col_(0), height_(0), width_(0) {}
  };

  class SeamCarverKeepout : public KSeamCarver {
  public:
    SeamCarverKeepout(int32_t row, int32_t col, int32_t height, int32_t width, double margin_energy = 390150.0) :
      KSeamCarver(margin_energy), keepoutRegionExists_(true), keepoutRegion_(row, col, height, width) {}

    SeamCarverKeepout(double margin_energy = 390150.0) :
      KSeamCarver(margin_energy), keepoutRegionExists_(false) {}

    /**
     * @brief find and remove vertical seams
     * @param numSeams number of vertical seams to remove
     * @param img input image
     * @param outImg output paramter
     * @param computeEnergy pointer to a user-defined energy function. If one is not provided, internal one will be used
     * @return bool indicates whether seam removal was successful or not
     */
    bool findAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn = nullptr);

    /**
     * @brief sets the keepout region
     * @param row starting row of keepout region (inclusive)
     * @param col starting column of keepout region (inclusive)
     * @param height how many rows tall is the keepout region
     * @param width how many columns wide is the keepout region
     */
    void setKeepoutRegion(int32_t row, int32_t col, int32_t height, int32_t width);

    /**
     * @brief deletes the keepout region
     */
    void deleteKeepoutRegion();
  protected:
    KeepoutRegionStruct keepoutRegion_;
    bool keepoutRegionExists_;
  };
 
}

#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <vector>

using std::vector;

namespace ct
{
  class KPixelEnergy2D
  {
  public:
    /**
     * @brief Default constructor, where the default pixel energy at the edges is 390150.0
     * @param
     */
    explicit KPixelEnergy2D(double MarginEnergy = 390150.0);

    /**
     * @brief CTOR that will initialize internal memory and 
     * @param
     * @param
     * @param
     */
    explicit KPixelEnergy2D(int32_t NumColumns, int32_t NumRows, double MarginEnergy = 390150.0);

    /**
     * @brief
     * @param
     * @param
     */
    explicit KPixelEnergy2D(const cv::Mat& PixelData, double MarginEnergy = 390150.0);

    /**
     * @brief
     * @return
     */
    double GetMarginEnergy() const;

    /**
     *
     */
    void SetMarginEnergy(double MarginEnergy);

    /**
     *
     */
    bool GetDimensions(int32_t& OutNumCols, int32_t& OutNumRows) const;

  protected:

  private:
    // width (in pixels) of images this class operates on
    int32_t NumColumns_ = 0;

    // height (in pixels) of images this class operates on
    int32_t NumRows_ = 0;

    // indicates whether image dimensions and memory has already been allocated
    bool bDimensionsInitialized = false;

    // energy at the borders of an image
    double MarginEnergy_ = 0.0;

    // number of channels used for computing energy of a BGR image
    const int32_t NumColorChannels_ = 3;
  };
}
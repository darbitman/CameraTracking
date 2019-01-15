#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <vector>
#include "ImageDimensionStruct.h"

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
     * @param NumColumns: width of the image in pixels
     * @param NumRows: height of the image in pixels
     * @param NumChannels: number of color channels in image (1 for grayscale, 3 for BGR color)
     * @param MarginEnergy: energy defined for border pixels
     */
    explicit KPixelEnergy2D(int32_t NumColumns, int32_t NumRows, int32_t NumChannels, double MarginEnergy = 390150.0);

    /**
     * @brief
     * @param
     * @param
     */
    explicit KPixelEnergy2D(const cv::Mat& Image, double MarginEnergy = 390150.0);

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
    bool GetDimensions(ImageDimensionStruct& OutImageDimensions) const;

    /**
     *
     */
    void SetDimensions(int32_t NumColumns, int32_t NumRows, int32_t NumChannels);

    /**
     *
     *
     */
    bool CalculatePixelEnergy(const cv::Mat& Image, vector< vector<double> >& OutPixelEnergy);

  protected:

  private:
    // stores number of columns, rows, color channels
    ImageDimensionStruct ImageDimensions;

    // energy at the borders of an image
    double MarginEnergy_ = 0.0;

    // indicates whether image dimensions and memory has already been allocated
    bool bDimensionsInitialized = false;

    // indicates whether the number of color channels has been established
    bool bNumChannelsInitialized = false;

    // number of channels used for computing energy of a BGR image
    const int32_t CNumChannelsInColorImage_ = 3;
  };
}
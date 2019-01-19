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
    virtual double GetMarginEnergy() const;

    /**
     *
     */
    virtual void SetMarginEnergy(double MarginEnergy);

    /**
     *
     */
    virtual bool GetDimensions(ImageDimensionStruct& OutImageDimensions) const;

    /**
     *
     */
    virtual void SetDimensions(int32_t NumColumns, int32_t NumRows, int32_t NumChannels);

    /**
     * @brief
     * @param Image: 2D matrix representation of the image
     * @param OutPixelEnergy: Out parameter, 2D vector of calculated pixel energies
     * @return bool: indicates if the operation was successful
     */
    virtual bool CalculatePixelEnergy(const cv::Mat& Image, vector< vector<double> >& OutPixelEnergy);

  protected:
    /**
     * @brief
     * @param Image: 2D matrix representation of the image
     * @param OutPixelEnergy: Out parameter, 2D vector of calculated pixel energies
     * @param bDoOddColumns: Indicates whether odd or even columns are done
     * @return bool: indicates if the operation was successful
     */
    virtual bool CalculatePixelEnergyForEveryRow(const cv::Mat& Image, vector< vector<double> >& OutPixelEnergy, bool bDoOddColumns);

    /**
     * @brief
     * @param Image: 2D matrix representation of the image
     * @param OutPixelEnergy: Out parameter, 2D vector of calculated pixel energies
     * @param bDoOddRows: Indicates whether odd or even rows are done
     * @return bool: indicates if the operation was successful
     */
    virtual bool CalculatePixelEnergyForEveryColumn(const cv::Mat& Image, vector< vector<double> >& OutPixelEnergy, bool bDoOddRows);

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
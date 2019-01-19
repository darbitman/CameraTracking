#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <queue>
#include "ConstSizeMinBinaryHeap.h"
#include "PixelEnergy2D.h"

using std::vector;

namespace ct {
  typedef void(*energyFunc)(const cv::Mat& img, vector<vector<double>>& outPixelEnergy);
  typedef vector<ConstSizeMinBinaryHeap<int32_t>> VectorOfMinPQ;

  class KSeamCarver {
  public:
    KSeamCarver(double MarginEnergy = 390150.0) : CMarginEnergy(MarginEnergy),
      NumRows_(0),
      NumColumns_(0),
      BottomRow_(0),
      RightColumn_(0),
      PosInf_(std::numeric_limits<double>::max()),
      PixelEnergyCalculator_(MarginEnergy) {}

    virtual ~KSeamCarver() {}

    /**
     * @brief find and remove vertical seams
     * @param NumSeams number of vertical seams to remove
     * @param img input image
     * @param outImg output paramter
     * @param computeEnergy pointer to a user-defined energy function. If one is not provided, internal one will be used
     * @return bool indicates whether seam removal was successful or not
     */
    virtual bool FindAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn = nullptr);

  protected:
    /**
     * @brief find vertical seams for later removal
     * @param PixelEnergy: calculated pixel energy of image
     * @param OutDiscoveredSeams: output parameter (vector of priority queues)
     * @return bool: indicates success
     */
    virtual bool FindVerticalSeams(int32_t NumSeams, vector<vector<double>>& PixelEnergy, VectorOfMinPQ& OutDiscoveredSeams);

    /**
    * @brief calculates the energy required to reach bottom row and saves the column of the pixel in the row above to get to every pixel
    * @param PixelEnergy: calculated pixel energy of image
    * @param OutTotalEnergyTo: cumulative energy to reach pixel
    * @param OutColumnTo: columnn of the pixel in the row above to get to every pixel
    */
    virtual void CalculateCumulativeVerticalPathEnergy(const vector<vector<double>>& PixelEnergy, vector<vector<double>>& OutTotalEnergyTo, vector<vector<int32_t>>& OutColumnTo);

    /**
     * @brief remove vertical seam from img given by column locations stored in seam
     * @param bgr image separate into 3 channels (BLUE GREEN RED)
     * @param seams vector of priority queues that hold the columns for the pixels to remove
     *              for each row, where the index into the vector is the row number
     */
    virtual void RemoveVerticalSeams(vector<cv::Mat>& bgr, VectorOfMinPQ& seams);
    
    /**
     * @brief helper function to mark where the seams are
     * @param bgr output parameter in which seams will be MarkedPixels
     * @param seams vector of priority queues that hold the columns for the pixels to remove
     *              for each row, where the index into the vector is the row number
     */
    virtual void markVerticalSeams(vector<cv::Mat>& bgr, VectorOfMinPQ& seams);

    /**
     * @brief helper function to mark where cumulative energy is MarkedPixels as +INF
     * @param bgr output parameter in which seams will be MarkedPixels
     * @param PixelEnergy calculated pixel energy of image
     */
    virtual void markInfEnergy(vector<cv::Mat>& bgr, vector<vector<double>>& pixelEnergy);

    // vector to store pixels that have been previously MarkedPixels for removal
    // will ignore these MarkedPixels pixels when searching for a new seam
    vector<vector<bool>> MarkedPixels;

    // default energy at the borders of the image
    const double CMarginEnergy;

    int32_t NumRows_;
    int32_t NumColumns_;
    int32_t BottomRow_;
    int32_t RightColumn_;
    double PosInf_;

    KPixelEnergy2D PixelEnergyCalculator_;
  };
 
}

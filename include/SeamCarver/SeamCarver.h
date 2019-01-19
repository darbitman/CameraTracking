#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <queue>
#include "ConstSizeMinBinaryHeap.h"
#include "PixelEnergy2D.h"

using std::vector;

namespace ct {
  typedef void(*energyFunc)(const cv::Mat& img, vector<vector<double>>& outPixelEnergy);
  typedef vector<ConstSizeMinBinaryHeap<int32_t>> vecMinPQ;

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
     * @param numSeams number of vertical seams to remove
     * @param img input image
     * @param outImg output paramter
     * @param computeEnergy pointer to a user-defined energy function. If one is not provided, internal one will be used
     * @return bool indicates whether seam removal was successful or not
     */
    virtual bool FindAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn = nullptr);

  protected:
    /**
     * @brief find vertical seam to remove
     * @param pixelEnergy calculated pixel energy of image
     * @param marked previously marked pixels for seam removal
     * @param outSeams output parameter (vector of priority queues)
     * @return bool indicates success
     */
    virtual bool findVerticalSeams(int32_t numSeams, vector< vector<double> >& pixelEnergy, vecMinPQ& outSeams);

    /**
    * @brief calculates the energy required to reach bottom row
    * @param pixelEnergy calculated pixel energy of image
    * @param marked pixels marked for seam removal
    * @param totalEnergyTo output parameter: total energy required to reach pixel at r,c
    * @param colTo previous row's column to get to current pixel at row,col
    */
    virtual void calculateVerticalPathEnergy(const vector< vector<double> >& pixelEnergy, vector< vector<double> >& totalEnergyTo, vector< vector<int32_t> >& colTo);

    /**
     * @brief remove vertical seam from img given by column locations stored in seam
     * @param bgr image separate into 3 channels (BLUE GREEN RED)
     * @param seams vector of priority queues that hold the columns for the pixels to remove
     *              for each row, where the index into the vector is the row number
     */
    virtual void removeVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams);
    
    /**
     * @brief helper function to mark where the seams are
     * @param bgr output parameter in which seams will be marked
     * @param seams vector of priority queues that hold the columns for the pixels to remove
     *              for each row, where the index into the vector is the row number
     */
    virtual void markVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams);

    /**
     * @brief helper function to mark where cumulative energy is marked as +INF
     * @param bgr output parameter in which seams will be marked
     * @param pixelEnergy calculated pixel energy of image
     */
    virtual void markInfEnergy(vector<cv::Mat>& bgr, vector<vector<double>>& pixelEnergy);

    // vector to store pixels that have been previously marked for removal
    // will ignore these marked pixels when searching for a new seam
    vector<vector<bool>> marked;

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

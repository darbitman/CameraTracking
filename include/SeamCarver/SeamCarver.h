#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>
#include <queue>
#include "ConstSizeMinBinaryHeap.h"

using std::vector;
using std::priority_queue;


namespace ct {
  typedef void(*energyFunc)(const cv::Mat& img, vector< vector<double> >& outPixelEnergy);
  //typedef vector< priority_queue<int32_t, vector<int32_t>, std::greater<int32_t> > > vecMinPQ;
  typedef vector< ConstSizeMinBinaryHeap<int32_t> > vecMinPQ;

  class SeamCarver {
  public:
    SeamCarver(double margin_energy = 390150.0) : MARGIN_ENERGY(margin_energy) {}

    ~SeamCarver() {}

    /**
     * @brief find and remove vertical seams
     * @param numSeams number of vertical seams to remove
     * @param img input image
     * @param outImg output paramter
     * @param computeEnergy pointer to a user-defined energy function. If one is not provided, internal one will be used
     * @return bool indicates whether seam removal was successful or not
     */
    bool findAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn = nullptr);

  protected:
    /**
     * @brief find vertical seam to remove
     * @param pixelEnergy calculated pixel energy of image
     * @param marked previously marked pixels for seam removal
     * @param outSeams output parameter (vector of priority queues)
     * @return bool indicates success
     */
    bool findVerticalSeams(int32_t numSeams, vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeams);

    /**
    * @brief calculates the energy required to reach bottom row
    * @param pixelEnergy calculated pixel energy of image
    * @param marked pixels marked for seam removal
    * @param totalEnergyTo output parameter: total energy required to reach pixel at r,c
    * @param colTo previous row's column to get to current pixel at row,col
    */
    void calculateVerticalPathEnergy(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vector< vector<double> >& totalEnergyTo, vector< vector<int32_t> >& colTo);

    /**
     * @brief remove vertical seam from img given by column locations stored in seam
     * @param bgr image separate into 3 channels (BLUE GREEN RED)
     * @param seams vector of priority queues that hold the columns for the pixels to remove
     *              for each row, where the index into the vector is the row number
     */
    void removeVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams);
    
    /**
     * @brief helper function to mark where the seams are
     * @param bgr output parameter in which seams will be marked
     * @param seams vector of priority queues that hold the columns for the pixels to remove
     *              for each row, where the index into the vector is the row number
     */
    void markVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams);

    /**
     * @brief helper function to mark where cumulative energy is marked as +INF
     * @param bgr output parameter in which seams will be marked
     * @param pixelEnergy calculated pixel energy of image
     */
    void markInfEnergy(vector<cv::Mat>& bgr, vector< vector<double> >& pixelEnergy);

    /**
     * @brief compute energy for every pixel for every row and choose whether to do odd/even columns
     * @param bgr image separate into 3 channels (BLUE GREEN RED)
     * @param outPixelEnergy output parameter that stores the energy for every pixel
     * @param oddColumns compute energy for odd columns or even columns
     */
    void energyForEveryRow(const vector<cv::Mat>& bgr, vector< vector<double> >& outPixelEnergy, bool oddColumns);

    /**
     * @brief compute energy for every pixel for every column and choose whether to do odd/even rows
     * @param bgr image separate into 3 channels (BLUE GREEN RED)
     * @param outPixelEnergy output parameter that stores the energy for every pixel
     * @param oddRows compute energy for odd rows or even rows
     */
    void energyForEveryColumn(const vector<cv::Mat>& bgr, vector< vector<double> >& outPixelEnergy, bool oddRows);

    // default energy at the borders of the image
    const double MARGIN_ENERGY;

    int32_t numRows;
    int32_t numCols;
    int32_t bottomRow;
    int32_t rightCol;
    double posInf;
  };
 
}

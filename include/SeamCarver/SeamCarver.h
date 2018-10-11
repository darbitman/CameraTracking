#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>
#include <queue>


using std::vector;
using std::priority_queue;


namespace ct {
  typedef void(*energyFunc)(const cv::Mat& img, vector< vector<double> >& outPixelEnergy);
  typedef vector< priority_queue<int32_t, vector<int32_t>, std::greater<int32_t> > > vecMinPQ;

  class SeamCarver {
  public:
    SeamCarver(double margin_energy = 624.6198844097104) : MARGIN_ENERGY(margin_energy) {}

    ~SeamCarver() {}

    /**
     * @brief find and remove vertical seams
     * @param numSeams number of vertical seams to remove
     * @param img input image
     * @param outImg output paramter
     * @param computeEnergy pointer to a user-defined energy function. If one is not provided, internal one will be used
     * @return bool indicates whether seam removal was successful or not
     */
    bool findAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergy = nullptr);

    /**
     * @brief remove horizontal seams
     * @param numSeams number of vertical seams to remove
     * @param img input image
     * @param outImg output paramter
     * @param computeEnergy pointer to a user-defined energy function. If one is not provided, internal one will be used
     * @return bool indicates whether seam removal was successful or not
     */
    bool removeHorizontalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergy = nullptr);

  private:
    /**
     * @brief find vertical seam to remove
     * @param pixelEnergy calculated pixel energy of image
     * @param marked previously marked pixels for seam removal
     * @param outSeams output paramter (vector of priority queues)
     * @return bool indicates success
     */
    bool findVerticalSeam(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeams);

    /**
     * @brief find horizontal seam to remove
     * @param pixelEnergy calculated pixel energy of image
     * @param outSeam output paramter
     */
    void findHorizontalSeam(const vector< vector<double> >& pixelEnergy, vector<int>& outSeam);

    /**
     * @brief remove vertical seam from img given by column locations stored in seam
     * @param bgr image separate into 3 channels (BLUE GREEN RED)
     * @param seams vector of priority queues that hold the columns for the pixels to remove
     *              for each row, where the index into the vector is the row number
     */
    void removeVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams);

    /**
     * @brief remove horizontal seam from img given by row locations stored in seam
     * @param bgr image separate into 3 channels (BLUE GREEN RED)
     * @param seam row locations of which pixel to remove, where the seam index is the column
     */
    void removeHorizontalSeam(vector<cv::Mat>& bgr, const vector<int>& seam);

    /**
     * @brief compute energy of pixel at [r, c]
     * @param bgr image separated into 3 channels (BLUE GREEN RED)
     * @param r pixel row
     * @param c pixel column
     * @param outEnergy output paramter
     * @return bool indicates whether computation was successful
     */
    bool energyAt(const vector<cv::Mat>& bgr, int32_t r, int32_t c, double& outEnergy);

    /**
     * @brief compute energy for every pixel of image
     * @param bgr image separate into 3 channels (BLUE GREEN RED)
     * @param outPixelEnergy output parameter
     */
    void energy(const vector<cv::Mat>& bgr, vector< vector<double> >& outPixelEnergy);

    // default energy at the borders of the image
    const double MARGIN_ENERGY;
  };

}

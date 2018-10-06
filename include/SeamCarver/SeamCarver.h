#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>


using std::vector;


namespace ct {
  typedef void (*energyFunc)(const cv::Mat& img, vector< vector<double> >& outPixelEnergy);

  class SeamCarver {
  public:
    SeamCarver(double margin_energy = 624.6198844097);
    ~SeamCarver();

    /**
     * @brief remove vertical seams
     * @param numSeams number of vertical seams to remove
     * @param img input image
     * @param outImg output paramter
     * @param computeEnergy pointer to a user-defined energy function. If one is not provided, internal one will be used
     * @return bool indicates whether seam removal was successful or not
     */
    bool removeVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergy = nullptr);

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
    // compute energy of pixel at [r, c]
    // returns the result of the calculation through output parameter
    // output parameter is passed by reference
    bool energyAt(const vector<cv::Mat>& bgr, int32_t r, int32_t c, double& outEnergy);

    // compute energy for every pixel
    // returns the result through output parameter
    // output parameter is passed by reference
    void energy(const vector<cv::Mat>& bgr, vector< vector<double> >& outPixelEnergy);

    /**
     * @brief find vertical seam to remove
     * @param pixelEnergy calculated pixel energy of image
     * @param outSeam output paramter
     */
    void findVerticalSeam(const vector< vector<double> >& pixelEnergy, vector<int>& outSeam);

    /**
     * @brief find horizontal seam to remove
     * @param pixelEnergy calculated pixel energy of image
     * @param outSeam output paramter
     */
    void findHorizontalSeam(const vector< vector<double> >& pixelEnergy, vector<int>& outSeam);

    // remove seam from img given by pixel locations stored in seam
    // returns the result of the seam removal through output parameter
    // output parameter is passed by reference
    void removeVerticalSeam(vector<cv::Mat>& bgr, const vector<int>& seam);

    // remove seam from img given by pixel locations stored in seam
    // returns the result of the seam removal through output paramter
    // output parameter is passed by reference
    void removeHorizontalSeam(vector<cv::Mat>& bgr, const vector<int>& seam);

    // default energy at the border
    const double MARGIN_ENERGY;
  };

}

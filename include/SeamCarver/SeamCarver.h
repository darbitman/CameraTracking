#pragma once
#include <opencv2/opencv.hpp>
#include <stdint.h>
#include <iostream>


using std::vector;


namespace ct {

  class SeamCarver {
  public:
    SeamCarver(double margin_energy = 624.6198844097);
    ~SeamCarver();
    bool removeVerticalSeams(int32_t numSeams, cv::Mat& img, cv::Mat& outImg);
    bool removeHorizontalSeams(int32_t numSeams, cv::Mat& img, cv::Mat& outImg);

  //private:
    // compute energy of pixel at [r, c]
    // returns the result of the calculation through output parameter
    // output parameter is passed by reference
    bool energyAt(const vector<cv::Mat>& bgr, int32_t r, int32_t c, double& outEnergy);

    // compute energy for every pixel
    // returns the result through output parameter
    // output parameter is passed by reference
    bool energy(const cv::Mat& pixels, vector< vector<double> >& outPixelEnergy);

    // find vertical seam to remove
    // returns the seam through output parameter
    // output parameter is passed by reference
    void findVerticalSeam(vector< vector<double> >& pixelEnergy, vector<int>& outSeam);

    // remove seam from img given by pixel locations stored in seam
    // returns the result of the seam removal through output parameter
    // output parameter is passed by reference
    bool removeVerticalSeam(cv::Mat& img, cv::Mat& outImg, vector<int>& seam);
    bool removeHorizontalSeam(cv::Mat& img, cv::Mat& outImg, vector<int>& seam);

    // default energy at the border
    const double MARGIN_ENERGY;
  };

}

#include "SeamCarver.h"


ct::SeamCarver::SeamCarver(double margin_energy) : MARGIN_ENERGY(margin_energy) {
}


ct::SeamCarver::~SeamCarver() {
}


bool ct::SeamCarver::removeVerticalSeams(int32_t numSeams, cv::Mat& img, cv::Mat& outImg) {
  return false;
}


bool ct::SeamCarver::removeHorizontalSeams(int32_t numSeams, cv::Mat& img, cv::Mat& outImg) {
  return false;
}



bool ct::SeamCarver::energyAt(const vector<cv::Mat>& bgr, int32_t r, int32_t c, double& outEnergy) {
  // if r or c out of bounds, then return false
  if (r < 0 || c < 0 || r > bgr[0].size().height - 1 || c > bgr[0].size().width - 1) {
    return false;
  }
  // return energy for border pixels
  if (r == 0 || c == 0 || r == bgr[0].size().height - 1 || c == bgr[0].size().width - 1) {
    outEnergy = this->MARGIN_ENERGY;
  }
  else {
    // compute energy for every pixel by computing gradient of colors
    // DeltaX = DeltaRx^2 + DeltaGx^2 + DeltaBx^2
    // DeltaY = DeltaRy^2 + DeltaGy^2 + DeltaBy^2
    // energy = sqrt(DeltaX + DeltaY)
    double deltaSquareX = (pow(bgr[2].at<char>(r, c + 1) - bgr[2].at<char>(r, c - 1), 2.0) +
                           pow(bgr[1].at<char>(r, c + 1) - bgr[1].at<char>(r, c - 1), 2.0) +
                           pow(bgr[0].at<char>(r, c + 1) - bgr[0].at<char>(r, c - 1), 2.0));
    double deltaSquareY = (pow(bgr[2].at<char>(r + 1, c) - bgr[2].at<char>(r - 1, c), 2.0) +
                           pow(bgr[1].at<char>(r + 1, c) - bgr[1].at<char>(r - 1, c), 2.0) +
                           pow(bgr[0].at<char>(r + 1, c) - bgr[0].at<char>(r - 1, c), 2.0));
    outEnergy = sqrt(deltaSquareX + deltaSquareY) / 624.6198844097; // normalize
  }
  return true;
}


bool ct::SeamCarver::energy(const cv::Mat& pixels, vector< vector<double> >& outPixelEnergy) {
  // split input pixels into separate blue, green, red channels
  vector<cv::Mat> bgr;
  bgr.resize(3);
  cv::split(pixels, bgr);

  // resize output if necessary
  if (outPixelEnergy.size() != pixels.size().height) {
    outPixelEnergy.resize(pixels.size().height);
  }
  if (outPixelEnergy[0].size() != pixels.size().width) {
    for (int i = 0; i < pixels.size().height; i++) {
      outPixelEnergy[i].resize(pixels.size().width);
    }
  }
  double computedEnergy = 0.0;
  for (int r = 0; r < pixels.size().height; r++) {
    for (int c = 0; c < pixels.size().width; c++) {
      if (this->energyAt(bgr, r, c, computedEnergy)) {
      }
      outPixelEnergy[r][c] = computedEnergy;
    }
  }
  return false;
}


void ct::SeamCarver::findVerticalSeam(const vector< vector<double> >& pixelEnergy, vector<int>& outSeam) {
  // totalEnergyTo array will store cumulative energy to each pixel
  // pixelTo array will store the pixel's columnn in the row above to get to current pixel
  vector< vector<double> > totalEnergyTo;
  vector< vector<int32_t> > colTo;

  // resize number of rows
  totalEnergyTo.resize(pixelEnergy.size());
  colTo.resize(pixelEnergy.size());

  // resize number of columns for each row
  for (uint32_t r = 0; r < pixelEnergy.size(); r++) {
    totalEnergyTo[r].resize(pixelEnergy[r].size());
    colTo[r].resize(pixelEnergy[r].size());
  }

  // initialize first row
  for (uint32_t c = 0; c < totalEnergyTo[0].size(); c++) {
    totalEnergyTo[0][c] = pixelEnergy[0][c];
    colTo[0][c] = -1;
  }

  for (uint32_t r = 1; r < pixelEnergy.size(); r++) {
    // energy at left and right margins
    // energy to current pixel is cumulative energy to previous pixel + energy of current pixel
    totalEnergyTo[r][0] = totalEnergyTo[r - 1][0] + pixelEnergy[r][0];
    totalEnergyTo[r][totalEnergyTo[r].size() - 1] =
      totalEnergyTo[r - 1][totalEnergyTo[r].size() - 1] + pixelEnergy[r][pixelEnergy[r].size() - 1];

    // previous pixel's column to get to the current pixel
    // save the previous column since we know that the previous row is just r - 1
    colTo[r][0] = 0;
    colTo[r][colTo[r].size() - 1] = colTo[r].size() - 1;

    // find shortest path to all pixels in the current row
    // i.e. for each column pixel in the current row
    double minEnergy = this->MARGIN_ENERGY;
    for (uint32_t c = 0; c < pixelEnergy[r].size(); c++) {
      int32_t minEnergyCol = c;
      // check if left/above is min
      if (c > 0) {
        if (totalEnergyTo[r - 1][c - 1] <= minEnergy) {
          minEnergy = totalEnergyTo[r - 1][c - 1];
          minEnergyCol = c - 1;
        }
      }

      // check if directly above is min
      if (totalEnergyTo[r - 1][c] <= minEnergy) {
        minEnergy = totalEnergyTo[r - 1][c];
        minEnergyCol = c;
      }

      // check if right/above is min
      if (c < pixelEnergy[r].size() - 1) {
        if (totalEnergyTo[r - 1][c + 1] <= minEnergy) {
          minEnergy = totalEnergyTo[r - 1][c + 1];
          minEnergyCol = c + 1;
        }
      }

      // assign cumulative energy to current pixel and the column of the previous pixel
      totalEnergyTo[r][c] = minEnergy + pixelEnergy[r][c];
      colTo[r][c] = minEnergyCol;
    }
  }

  // find minimum total energy endpoint
  // initialize total energy to the bottom-lefthand corner
  // will find a pixel that is in the seam of least total energy (if it exists)
  int32_t bottomRow = totalEnergyTo.size() - 1;
  double minTotalEnergy = totalEnergyTo[bottomRow][0];
  int32_t minTotalEnergyCol = 0;
  for (uint32_t c = 1; c < totalEnergyTo[bottomRow].size(); c++) {
    if (totalEnergyTo[bottomRow][c] < minTotalEnergy) {
      minTotalEnergy = totalEnergyTo[bottomRow][c];
      minTotalEnergyCol = c;
    }
  }

  /*** FIND SEAM ***/

  // ensure output vector is big enough to store the height of the pixel array (i.e. number of rows)
  outSeam.resize(totalEnergyTo.size());

  // set last pixel's column as endpoint of seam
  // found in previous step looking for min total energy endpoint
  outSeam[bottomRow] = minTotalEnergyCol;
  int col = 0;
  // trace path backwards
  for (int32_t r = bottomRow - 1; r >= 0; r--) {
    // save column of pixel to remove at previous row
    col = outSeam[r + 1];

    // using the previous pixel's row and column, extract the column of the pixel in the current row
    //   that will be removed as part of the "min" energy seam
    // save the column of the pixel in the current row
    outSeam[r] = colTo[r + 1][col];
  }
}


void ct::SeamCarver::findHorizontalSeam(const vector< vector<double> >& pixelEnergy, vector<int>& outSeam) {
  // totalEnergyTo array will store cumulative energy to each pixel
  // pixelTo array will store the pixel's row in the column to the left to get to current pixel

  vector< vector<double> > totalEnergyTo;
  vector< vector<int32_t> > rowTo;

  // resize number of rows
  totalEnergyTo.resize(pixelEnergy.size());
  rowTo.resize(pixelEnergy.size());

  // resize number of columns for each row
  for (uint32_t r = 0; r < pixelEnergy.size(); r++) {
    totalEnergyTo[r].resize(pixelEnergy[r].size());
    rowTo[r].resize(pixelEnergy[r].size());
  }

  // initialize first column
  for (uint32_t r = 0; r < totalEnergyTo.size(); r++) {
    totalEnergyTo[r][0] = pixelEnergy[r][0];
    rowTo[r][0] = 01;
  }

  for (uint32_t c = 1; c < pixelEnergy[0].size(); c++) {
    // energy at top and bottom margins
    // energy to current pixel is cumulative energy to previous pixel + energy of current pixel
    totalEnergyTo[0][c] = totalEnergyTo[0][c - 1] + pixelEnergy[0][c];
    totalEnergyTo[totalEnergyTo.size() - 1][c] =
      totalEnergyTo[totalEnergyTo.size() - 1][c - 1] + pixelEnergy[pixelEnergy.size() - 1][c];

    // previous pixel's row to get to the current pixel
    // save the previous row since we know that the previous column is just c - 1
    rowTo[0][c] = 0;
    rowTo[rowTo.size() - 1][c] = rowTo.size() - 1;

    // find shortest path to all pixels in the current column
    // i.e. for each row pixel in the current column
    double minEnergy = this->MARGIN_ENERGY;
    for (uint32_t r = 0; r < pixelEnergy.size(); r++) {
      int32_t minEnergyRow = r;
      // check if left/above is min
      if (r > 0) {
        if (totalEnergyTo[r - 1][c - 1] <= minEnergy) {
          minEnergy = totalEnergyTo[r - 1][c - 1];
          minEnergyRow = r - 1;
        }
      }

      // check if directly left is min
      if (totalEnergyTo[r][c - 1] <= minEnergy) {
        minEnergy = totalEnergyTo[r][c - 1];
        minEnergyRow = r;
      }

      // check if left/below is min
      if (r < pixelEnergy.size() - 1) {
        if (totalEnergyTo[r + 1][c - 1]) {
          minEnergy = totalEnergyTo[r + 1][c - 1];
          minEnergyRow = r + 1;
        }
      }

      // assign cumulative energy to current pixel and the row of the previous pixel
      totalEnergyTo[r][c] = minEnergy + pixelEnergy[r][c];
      rowTo[r][c] = minEnergyRow;
    }
  }

  // find minimum total energy endpoint
  // initialize total energy to the upper-righthand corner
  // will find a pixel that is in the seam of least total energy (if it exists)
  int32_t rightCol = totalEnergyTo[0].size() - 1;
  double minTotalEnergy = totalEnergyTo[0][rightCol];
  int32_t minTotalEnergyRow = 0;
  for (uint32_t r = 1; r < totalEnergyTo.size(); r++) {
    if (totalEnergyTo[r][rightCol] < minTotalEnergy) {
      minTotalEnergy = totalEnergyTo[r][rightCol];
      minTotalEnergyRow = r;
    }
  }

    /*** FIND SEAM ***/
    // ensure output vector is big enough to store the width of the pixel array (i.e. number of columns)
    outSeam.resize(totalEnergyTo[0].size());

    // set last pixel's row as endpoint of seam
    // found in previous step looking for min total energy endpoint
    outSeam[rightCol] = minTotalEnergyRow;
    int row = 0;
    // trace path backwards
    for (int32_t c = rightCol - 1; c >= 0; c--) {
      // save row of pixel to remove at previous column
      row = outSeam[c + 1];

      // using the previous pixel's row and column, extract the row of the pixel in the current column
      //   that will be removed as part of the "min" energy seam
      // save the row of the pixel in the current column
      outSeam[c] = rowTo[row][c + 1];
    }
}


bool ct::SeamCarver::removeVerticalSeam(cv::Mat& img, cv::Mat& outImg, vector<int>& seam) {
  return false;
}


bool ct::SeamCarver::removeHorizontalSeam(cv::Mat& img, cv::Mat& outImg, vector<int>& seam) {
  return false;
}
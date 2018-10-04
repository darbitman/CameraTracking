#include "SeamCarver.h"


ct::SeamCarver::SeamCarver(double margin_energy) : MARGIN_ENERGY(margin_energy) {
}


ct::SeamCarver::~SeamCarver() {
}


void ct::SeamCarver::removeVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg) {
  outImg = img.clone();

  // split BGR channels
  vector<cv::Mat> bgr;
  bgr.resize(3);
  vector< vector<double> > energy;
  vector<int> seam;

  // remove seams loop
  for (int i = 0; i < numSeams; i++) {
    // split img into 3 channels
    cv::split(outImg, bgr);

    // compute energy of pixels
    this->energy(bgr, energy);

    // find vertical seam
    this->findVerticalSeam(energy, seam);
    
    // remove the seam
    this->removeVerticalSeam(bgr, seam);

    // merge 3 channels into final image
    cv::merge(bgr, outImg);
  }
}


void ct::SeamCarver::removeHorizontalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg) {
  outImg = img.clone();

  // split BGR channels
  vector<cv::Mat> bgr;
  bgr.resize(3);
  vector< vector<double> > energy;
  vector<int> seam;

  // remove seams loop
  for (int i = 0; i < numSeams; i++) {
    // split img into 3 channels
    cv::split(outImg, bgr);

    // compute energy of pixels
    this->energy(bgr, energy);

    // find vertical seam
    this->findHorizontalSeam(energy, seam);

    // remove the seam
    this->removeHorizontalSeam(bgr, seam);

    // merge 3 channels into final image
    cv::merge(bgr, outImg);
  }
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


bool ct::SeamCarver::energy(const vector<cv::Mat>& bgr, vector< vector<double> >& outPixelEnergy) {
  // resize output if necessary
  if (outPixelEnergy.size() != bgr[0].size().height) {
    outPixelEnergy.resize(bgr[0].size().height);
  }
  if (outPixelEnergy[0].size() != bgr[0].size().width) {
    for (int i = 0; i < bgr[0].size().height; i++) {
      outPixelEnergy[i].resize(bgr[0].size().width);
    }
  }
  double computedEnergy = 0.0;
  for (int r = 0; r < bgr[0].size().height; r++) {
    for (int c = 0; c < bgr[0].size().width; c++) {
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

    // previous pixel used to get to current pixel at left and right margins
    // previous pixel's column to get to the current pixel
    // save the previous column since we know that the previous row is just r - 1
    colTo[r][0] = 0;
    colTo[r][colTo[r].size() - 1] = colTo[r].size() - 1;

    // find shortest path to all pixels in the current row
    // i.e. for each column pixel in the current row
    double minEnergy = this->MARGIN_ENERGY;
    for (uint32_t c = 0; c < pixelEnergy[r].size(); c++) {
      // initialize minEnergy to pixel above
      // will check if left/above or right/above is less
      minEnergy = totalEnergyTo[r - 1][c];
      int32_t minEnergyCol = c;
      // check if left/above is min
      if (c > 0) {
        if (totalEnergyTo[r - 1][c - 1] < minEnergy) {
          minEnergy = totalEnergyTo[r - 1][c - 1];
          minEnergyCol = c - 1;
        }
      }

      // check if right/above is min
      if (c < pixelEnergy[r].size() - 1) {
        if (totalEnergyTo[r - 1][c + 1] < minEnergy) {
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

  // resize each row to the number of columns
  for (uint32_t r = 0; r < pixelEnergy.size(); r++) {
    totalEnergyTo[r].resize(pixelEnergy[r].size());
    rowTo[r].resize(pixelEnergy[r].size());
  }

  // initialize first column
  for (uint32_t r = 0; r < totalEnergyTo.size(); r++) {
    totalEnergyTo[r][0] = pixelEnergy[r][0];
    rowTo[r][0] = -1;
  }

  for (uint32_t c = 1; c < pixelEnergy[0].size(); c++) {
    // energy at top and bottom margins
    // energy to current pixel is cumulative energy to previous pixel + energy of current pixel
    totalEnergyTo[0][c] = totalEnergyTo[0][c - 1] + pixelEnergy[0][c];
    totalEnergyTo[totalEnergyTo.size() - 1][c] =
      totalEnergyTo[totalEnergyTo.size() - 1][c - 1] + pixelEnergy[pixelEnergy.size() - 1][c];

    // previous pixel used to get to current pixel at top and bottom margin
    // previous pixel's row to get to the current pixel
    // save the previous row since we know that the previous column is just c - 1
    rowTo[0][c] = 0;
    rowTo[rowTo.size() - 1][c] = rowTo.size() - 1;

    // find shortest path to all pixels in the current column
    // i.e. for each row pixel in the current column
    double minEnergy = this->MARGIN_ENERGY;
    for (uint32_t r = 0; r < pixelEnergy.size(); r++) {
      // initialize minEnergy to left
      // will check if left/above or left/below is less
      minEnergy = totalEnergyTo[r][c - 1];
      int32_t minEnergyRow = r;
      // check if left/above is min
      if (r > 0) {
        if (totalEnergyTo[r - 1][c - 1] < minEnergy) {
          minEnergy = totalEnergyTo[r - 1][c - 1];
          minEnergyRow = r - 1;
        }
      }

      // check if left/below is min
      if (r < pixelEnergy.size() - 1) {
        if (totalEnergyTo[r + 1][c - 1] < minEnergy) {
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


void ct::SeamCarver::removeVerticalSeam(vector<cv::Mat>& bgr, const vector<int>& seam) {
  // for every channel (BGR) move pixels over to the left by one starting at the seam effectively removing the seam
  for (int32_t i = 0; i < 3; i++) {
    for (int32_t r = 0; r < bgr[i].size().height; r++) {
      for (int32_t c = seam[r]; c < bgr[i].size().width - 2; c++) {
        bgr[i].at<char>(r, c) = bgr[i].at<char>(r, c + 1);
      }
    }
  }

  // remove last column
  for (int32_t i = 0; i < 3; i++) {
    bgr[i] = bgr[i].colRange(0, bgr[i].cols - 2);
  }
}


void ct::SeamCarver::removeHorizontalSeam(vector<cv::Mat>& bgr, const vector<int>& seam) {
  // for every channel (BGR) move pixels up by one starting at the seam effectively removing the seam
  for (int32_t i = 0; i < 3; i++) {
    for (int32_t c = 0; c < bgr[i].size().width; c++) {
      for (int32_t r = seam[c]; r < bgr[i].size().height - 2; r++) {
        bgr[i].at<char>(r, c) = bgr[i].at<char>(r + 1, c);
      }
    }
  }

  // remove bottom row
  for (int32_t i = 0; i < 3; i++) {
    bgr[i] = bgr[i].rowRange(0, bgr[i].rows - 2);
  }
}
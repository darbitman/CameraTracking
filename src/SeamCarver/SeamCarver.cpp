#include "SeamCarver.h"
#include <limits>


bool ct::SeamCarver::removeVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergy) {
  // check if removing more seams than columns available
  if (numSeams > img.size().width) {
    return false;
  }

  /*** declare vectors that will be used throughout the seam removal process ***/
  // output of the function to compute energy
  // input to the seam finding function
  vector< vector<double> > pixelEnergy;

  // output of the seam finding function
  // input to the seam removal function
  // vector of minimum-oriented priority queues. Each row in the vector corresponds to a priority queue for that row in the image
  vecMinPQ seams;
  seams.resize(img.size().height);

  // vector to store pixels that have been previously marked for removal
  // will ignore these marked pixels when searching for a new seam
  vector< vector<bool> > marked;

  // resize marked matrix to the same size as img
  marked.resize(img.size().height);
  for (int32_t r = 0; r < img.size().height; r++) {
    marked[r].resize(img.size().width);
  }

  // initialize marked matrix to false
  for (int32_t r = 0; r < img.size().height; r++) {
    for (int32_t c = 0; c < img.size().width; c++) {
      marked[r][c] = false;
    }
  }
 
  outImg = img.clone();

  // vector to store the image's channels separately
  vector<cv::Mat> bgr;
  bgr.resize(3);

  try {
    // compute energy of pixels
    if (computeEnergy == nullptr) {
      // split img into 3 channels (BLUE, GREEN, RED)
      cv::split(outImg, bgr);

      // call built-in energy computation function
      this->energy(bgr, pixelEnergy);
    }
    else {
      // call user-defined energy computation function
      computeEnergy(outImg, pixelEnergy);
    }
    //cv::namedWindow("test");
    
    for (int32_t i = 0; i < numSeams; i++) {
      this->findVerticalSeam(pixelEnergy, marked, seams);
      //for (int r = 0; r < img.size().height; r++) {
      //  for (int j = 0; j < 3; j++) {
      //    bgr[j].at<uchar>(r, seams[i][r]) = 0;
      //  }
      //}
      //cv::merge(bgr, outImg);
      //cv::imshow("test", outImg);
    }

    /*** sort seam vectors according to their first element ***/
    //struct {
    //  bool operator()(const vector<int32_t>& a, const vector<int32_t>& b) {
    //    return a[0] < b[0];
    //  }
    //} compFunc;
    //std::sort(seams.begin(), seams.end(), compFunc);

    int32_t col = 0;
    for (int32_t r = 0; r < seams.size(); r++) {
      int32_t count = 1;
      while (seams[r].size()) {
        col = seams[r].top();
        seams[r].pop();
        int32_t rightBorder = (seams[r].size() ? seams[r].top() : img.size().width);
        for (int c = col + 1; c < rightBorder; c++) {
          for (int j = 0; j < 3; j++) {
            bgr[j].at<uchar>(r, c - count) = bgr[j].at<uchar>(r, c);
          }
        }
        count++;
      }
    }

    cv::merge(bgr, outImg);

    //// remove multiple seams
    //for (int i = 0; i < numSeams; i++) {
    //  // find vertical seam
    //  this->findVerticalSeam(energy, seam);

    //  // remove the seam
    //  this->removeVerticalSeam(bgr, seam);

    //  // merge 3 channels into final image
    //  cv::merge(bgr, outImg);
    //}
  }
  catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    return false;
  }

  return true;
}


bool ct::SeamCarver::removeHorizontalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergy) {
  /*** declare vectors that will be used throughout the seam removal process ***/
  // output of the function to compute energy
  // input to the seam finding function
  vector< vector<double> > energy;

  // output of the seam finding function
  // input to the seam removal function
  vector<int> seam;

  outImg = img.clone();

  // vector to store the result of the channel splitting function
  vector<cv::Mat> bgr;
  bgr.resize(3);

  try {
    // remove multiple seams
    for (int i = 0; i < numSeams; i++) {

      // compute energy of pixels
      if (computeEnergy == nullptr) {
        // split img into 3 channels (BLUE, GREEN, RED)
        cv::split(outImg, bgr);

        // call built-in energy computation function
        this->energy(bgr, energy);
      }
      else {
        // call user-defined energy computation function
        computeEnergy(outImg, energy);
      }

      // find vertical seam
      this->findHorizontalSeam(energy, seam);

      // remove the seam
      this->removeHorizontalSeam(bgr, seam);

      // merge 3 channels into final image
      cv::merge(bgr, outImg);
    }
  }
  catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    return false;
  }
  return true;
}


bool ct::SeamCarver::findVerticalSeam(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeam) {
  if (pixelEnergy.size() == 0) {
    throw std::out_of_range("Pixel energy vector is empty\n");
  }

  if (outSeam.size() != pixelEnergy.size()) {
    throw std::out_of_range("outSeam does not have enough rows\n");
  }

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

  // initialize top row
  for (uint32_t c = 0; c < totalEnergyTo[0].size(); c++) {
    // if previously marked, set its energy to +INF
    if (marked[0][c]) {
      totalEnergyTo[0][c] = std::numeric_limits<double>::max();
    }
    else {
      totalEnergyTo[0][c] = pixelEnergy[0][c];
    }
    colTo[0][c] = -1;
  }

  for (uint32_t r = 1; r < pixelEnergy.size(); r++) {
    //// energy at left and right margins
    //// energy to current pixel is cumulative energy to previous pixel + energy of current pixel
    //// if previously marked, set energy to +INF
    //if (!marked[r][0]) {
    //  totalEnergyTo[r][0] = totalEnergyTo[r - 1][0] + pixelEnergy[r][0];
    //}
    //else {
    //  totalEnergyTo[r][0] = std::numeric_limits<double>::max();
    //}

    //if (!marked[r][totalEnergyTo[r].size() - 1]) {
    //  totalEnergyTo[r][totalEnergyTo[r].size() - 1] =
    //    totalEnergyTo[r - 1][totalEnergyTo[r].size() - 1] + pixelEnergy[r][pixelEnergy[r].size() - 1];
    //}
    //else {
    //  totalEnergyTo[r][totalEnergyTo[r].size() - 1] = std::numeric_limits<double>::max();
    //}

    //// previous pixel used to get to current pixel at left and right margins
    //// previous pixel's column to get to the current pixel
    //// save the previous column since we know that the previous row is just r - 1
    //colTo[r][0] = 0;
    //colTo[r][colTo[r].size() - 1] = colTo[r].size() - 1;

    // find minimum energy path from previous row to every pixel in the current row
    // initialize min energy to +INF
    // initialize the previous column to -1 to set error state
    double minEnergy = std::numeric_limits<double>::max();
    int32_t minEnergyCol = -1;
    for (uint32_t c = 0; c < pixelEnergy[r].size(); c++) {
      minEnergy = std::numeric_limits<double>::max();
      minEnergyCol = -1;

      // save some cycles by not doing any comparisons if the current pixel has been previously marked
      if (!marked[r][c]) {
        //// initialize minEnergy to pixel above
        //// will check if left/above or right/above is less
        //minEnergy = totalEnergyTo[r - 1][c];

        // check above
        if (!marked[r - 1][c] && totalEnergyTo[r - 1][c] < minEnergy) {
          minEnergy = totalEnergyTo[r - 1][c];
          minEnergyCol = c;
        }

        // check if left/above is min
        if (c > 0) {
          if (!marked[r - 1][c - 1] && totalEnergyTo[r - 1][c - 1] < minEnergy) {
            minEnergy = totalEnergyTo[r - 1][c - 1];
            minEnergyCol = c - 1;
          }
        }

        // check if right/above is min
        if (c < pixelEnergy[r].size() - 1) {
          if (!marked[r - 1][c + 1] && totalEnergyTo[r - 1][c + 1] < minEnergy) {
            minEnergy = totalEnergyTo[r - 1][c + 1];
            minEnergyCol = c + 1;
          }
        }
      }

      // assign cumulative energy to current pixel and save the column of the parent pixel
      if (minEnergyCol == -1) {
        // current pixel is unreachable from parent pixels since they are all marked
        // set energy to reach current pixel to +INF
        totalEnergyTo[r][c] = std::numeric_limits<double>::max();
      }
      else {
        totalEnergyTo[r][c] = minEnergy + pixelEnergy[r][c];
      }
      colTo[r][c] = minEnergyCol;
    }
  }

  // find one endpoint of least cumulative energy
  // initialize total energy to +INF to perform linear search
  // will find a pixel that is in the seam of least total energy (if it exists)
  int32_t bottomRow = totalEnergyTo.size() - 1;
  double minTotalEnergy = std::numeric_limits<double>::max();
  int32_t minTotalEnergyCol = -1;
  for (uint32_t c = 0; c < totalEnergyTo[bottomRow].size(); c++) {
    if (!marked[bottomRow][c] && totalEnergyTo[bottomRow][c] < minTotalEnergy) {
      minTotalEnergy = totalEnergyTo[bottomRow][c];
      minTotalEnergyCol = c;
    }
  }

  // just as a precaution if for some reason all pixels in bottom row have been marked
  if (minTotalEnergyCol == -1) {
    return false;
  }

  /*** FIND SEAM BY TRACING BACKWARDS ***/

  // set bottom row's pixel column as endpoint of seam
  // found in previous step looking for min total energy endpoint
  outSeam[bottomRow].push(minTotalEnergyCol);
  
  // mark the column of the pixel in the bottom row
  marked[bottomRow][minTotalEnergyCol] = true;

  int32_t col = minTotalEnergyCol;
  // trace path upwards
  for (int32_t r = bottomRow - 1; r >= 0; r--) {
    // using the below pixel's row and column, extract the column of the pixel in the current row
    //   that will be removed as part of the "min" energy seam
    // save the column of the pixel in the current row
    outSeam[r].push(colTo[r + 1][col]);

    // update to current column
    col = colTo[r + 1][col];

    // mark the column of the pixel in the current row
    marked[r][col] = true;
  }

  return true;
}


void ct::SeamCarver::findHorizontalSeam(const vector< vector<double> >& pixelEnergy, vector<int>& outSeam) {
  if (pixelEnergy.size() == 0) {
    throw std::out_of_range("Pixel energy vector is empty\n");
  }

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
    bgr[i] = bgr[i].colRange(0, bgr[i].cols - 1);
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
    bgr[i] = bgr[i].rowRange(0, bgr[i].rows - 1);
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
    double deltaSquareX = (pow(bgr[2].at<uchar>(r, c + 1) - bgr[2].at<uchar>(r, c - 1), 2.0) +  // DeltaRx^2
                           pow(bgr[1].at<uchar>(r, c + 1) - bgr[1].at<uchar>(r, c - 1), 2.0) +  // DeltaGx^2
                           pow(bgr[0].at<uchar>(r, c + 1) - bgr[0].at<uchar>(r, c - 1), 2.0));  // DeltaBx^2
    double deltaSquareY = (pow(bgr[2].at<uchar>(r + 1, c) - bgr[2].at<uchar>(r - 1, c), 2.0) +  // DeltaRy^2
                           pow(bgr[1].at<uchar>(r + 1, c) - bgr[1].at<uchar>(r - 1, c), 2.0) +  // DeltaGy^2
                           pow(bgr[0].at<uchar>(r + 1, c) - bgr[0].at<uchar>(r - 1, c), 2.0));  // DeltaBy^2
    outEnergy = sqrt(deltaSquareX + deltaSquareY);
  }
  return true;
}


void ct::SeamCarver::energy(const vector<cv::Mat>& bgr, vector< vector<double> >& outPixelEnergy) {
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
}
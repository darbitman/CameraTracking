#include "SeamCarver.h"
#include <limits>
#include <chrono>
#include <thread>
using namespace std::chrono;
using std::thread;

bool ct::SeamCarver::findAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn) {
  this->numRows = img.size().height;
  this->numCols = img.size().width;

  // check if removing more seams than columns available
  if (numSeams > numCols) {
    return false;
  }

  /*** DECLARE VECTORS THAT WILL BE USED THROUGHOUT THE SEAM REMOVAL PROCESS ***/
  // output of the function to compute energy
  // input to the seam finding function
  vector< vector<double> > pixelEnergy;

  // output of the seam finding function
  // input to the seam removal function
  // vector of minimum-oriented priority queues. Each row in the vector corresponds to a priority queue for that row in the image
  vecMinPQ seams;
  seams.resize(numRows);

  // vector to store pixels that have been previously marked for removal
  // will ignore these marked pixels when searching for a new seam
  vector< vector<bool> > marked;

  // resize marked matrix to the same size as img;
  // resize on a vector of bools initializes its elements to false by default
  marked.resize(numRows);
  for (int32_t r = 0; r < numRows; r++) {
    marked[r].resize(numCols);
  }

  // vector to store the image's channels separately
  vector<cv::Mat> bgr;
  bgr.resize(3);

  try {
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // compute energy of pixels
    if (computeEnergyFn == nullptr) {
      // split img into 3 channels (BLUE, GREEN, RED)
      start = high_resolution_clock::now();
      cv::split(img, bgr); // ~300-400us
      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);

      // call built-in energy computation function
      start = high_resolution_clock::now();
      this->energy(bgr, pixelEnergy); // ~165ms
      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);
    }
    else {
      // call user-defined energy computation function
      computeEnergyFn(img, pixelEnergy);
    }

    // find all vertical seams
    start = high_resolution_clock::now();
    this->findVerticalSeams(numSeams, pixelEnergy, marked, seams); // ~1.2s
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);

    // remove all found seams
    start = high_resolution_clock::now();
    //this->markVerticalSeams(bgr, seams);
    this->removeVerticalSeams(bgr, seams);  // ~65ms
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);

    // combine separate channels into output image
    start = high_resolution_clock::now();
    cv::merge(bgr, outImg); // ~300-400us
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
  }
  catch (std::exception e) {
    std::cout << e.what() << std::endl;
    //this->markVerticalSeams(bgr, seams);
    //this->markInfEnergy(bgr, pixelEnergy);
    //cv::merge(bgr, outImg);
    return false;
  }

  return true;
}


bool ct::SeamCarver::findAndRemoveHorizontalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergy) {
  return true;
}


bool ct::SeamCarver::findVerticalSeams(int32_t numSeams, vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeams) {
  if (pixelEnergy.size() == 0) {
    throw std::out_of_range("Pixel energy vector is empty\n");
  }

  if (outSeams.size() != pixelEnergy.size()) {
    throw std::out_of_range("outSeams does not have enough rows\n");
  }

  int32_t count = 0;

  // initialize constants to be used throughout function
  int32_t bottomRow = numRows - 1;
  double posInf = std::numeric_limits<double>::max();

  // totalEnergyTo array will store cumulative energy to each pixel
  // colTo array will store the pixel's columnn in the row above to get to current pixel
  vector< vector<double> > totalEnergyTo;
  vector< vector<int32_t> > colTo;

  // resize number of rows
  totalEnergyTo.resize(numRows);
  colTo.resize(numRows);

  // resize number of columns for each row
  for (int32_t r = 0; r < numRows; r++) {
    totalEnergyTo[r].resize(numCols);
    colTo[r].resize(numCols);
  }

  // initial path calculation
  this->calculateVerticalPathEnergy(pixelEnergy, marked, totalEnergyTo, colTo);

  // temporary seam to verify that there are no previously marked pixels in this seam
  // otherwise the cumulative energies need to be recalculated
  vector<int32_t> seam;
  seam.resize(numRows);

  // declare variables used in seam discovery when looking for the least cumulative energy column in the bottom row
  double minTotalEnergy = posInf;
  int32_t minTotalEnergyCol = -1;

  /*** RUN SEAM DISCOVERY ***/
  for (int32_t n = 0; n < numSeams; n++) {
    // find least cumulative energy column in bottom row
    // initialize total energy to +INF and run linear search for a pixel of least cumulative energy (if one exists)
    minTotalEnergy = posInf;
    minTotalEnergyCol = -1;
    for (int32_t c = 0; c < numCols; c++) {
      if (!marked[bottomRow][c] && totalEnergyTo[bottomRow][c] < minTotalEnergy) {
        minTotalEnergy = totalEnergyTo[bottomRow][c];
        minTotalEnergyCol = c;
      }
    }

    // all pixels in bottom row are unreachable due to +INF cumulative energy to all of them
    // therefore need to recalculate cumulative energies
    if (minTotalEnergyCol == -1) {
      // decrement seam number iterator since this seam was invalid
      // need to recalculate the cumulative energy
      n--;
      count++;
      this->calculateVerticalPathEnergy(pixelEnergy, marked, totalEnergyTo, colTo);
      std::cout << "recalculated seam: " << n + 1 << std::endl;
      goto continueSeamFindingLoop;
    }

    // save last column as part of seam
    seam[bottomRow] = minTotalEnergyCol;

    int32_t col = minTotalEnergyCol;
    int32_t currentCol = col;
    for (int32_t r = bottomRow - 1; r >= 0; r--) {
      // using the below pixel's row and column, extract the column of the pixel in the current row
      currentCol = colTo[r + 1][col];

      // check if another path of least cumulative energy can be found without recalculating energies
      if (marked[r][currentCol]) {
        // mark the starting pixel in bottom row as having +INF cumulative energy so it will not be chosen again
        totalEnergyTo[bottomRow][minTotalEnergyCol] = posInf;
        // decrement seam number iterator since this seam was invalid
        n--;
        // restart seam finding loop
        goto continueSeamFindingLoop;
      }

      // save the column of the pixel in the current row
      seam[r] = currentCol;

      // update to current column
      col = currentCol;
    }

    // copy seam and mark appropriate pixels
    for (int32_t r = 0; r < numRows; r++) {
      col = seam[r];
      outSeams[r].push(col);
      marked[r][col] = true;
    }

    continueSeamFindingLoop: {
      continue;
    }
  }
  std::cout << "recalculated total times: " << count << std::endl;
  return true;
}


void ct::SeamCarver::findHorizontalSeams(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeams) {}


void ct::SeamCarver::calculateVerticalPathEnergy(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vector< vector<double> >& totalEnergyTo, vector< vector<int32_t> >& colTo) {
  // initialize constants to be used throughout function
  double posInf = std::numeric_limits<double>::max();

  // initialize top row
  for (int32_t c = 0; c < numCols; c++) {
    // if previously marked, set its energy to +INF
    if (marked[0][c]) {
      totalEnergyTo[0][c] = posInf;
    }
    else {
      totalEnergyTo[0][c] = this->MARGIN_ENERGY;
    }
    colTo[0][c] = -1;
  }

  // cache the total energy to the pixels up/left, directly above, and up/right
  //   instead of accessing memory for the same pixels
  // shift energy values to the left and access memory only once
  // SHIFT OPERATION:
  //   left/above <== directly above
  //   directly above <== right/above
  //   right/above = access new memory
  double energyUpLeft = posInf;
  double energyUp = posInf;
  double energyUpRight = posInf;

  bool markedUpLeft = false;
  bool markedUp = false;
  bool markedUpRight = false;

  double minEnergy = posInf;
  int32_t minEnergyCol = -1;

  for (int32_t r = 1; r < numRows; r++) {
    energyUpLeft = posInf;
    energyUp = totalEnergyTo[r - 1][0];
    energyUpRight = numCols > 1 ? totalEnergyTo[r - 1][1] : posInf;

    markedUpLeft = true;
    markedUp = marked[r - 1][0];
    markedUpRight = numCols > 1 ? marked[r - 1][1] : true;

    // find minimum energy path from previous row to every pixel in the current row
    for (int32_t c = 0; c < numCols; c++) {
      // initialize min energy to +INF and initialize the previous column to -1
      //   to set error state
      minEnergy = posInf;
      minEnergyCol = -1;

      // save some cycles by not doing any comparisons if the current pixel has been previously marked
      if (!marked[r][c]) {
        // check above
        if (!markedUp && energyUp < minEnergy) {
          minEnergy = totalEnergyTo[r - 1][c];
          minEnergyCol = c;
        }

        // check if right/above is min
        if (c < numCols - 1) {
          if (!markedUpRight && energyUpRight < minEnergy) {
            minEnergy = totalEnergyTo[r - 1][c + 1];
            minEnergyCol = c + 1;
          }
        }

        // check if left/above is min
        if (c > 0) {
          if (!markedUpLeft && energyUpLeft < minEnergy) {
            minEnergy = totalEnergyTo[r - 1][c - 1];
            minEnergyCol = c - 1;
          }
        }
      }

      // shift energy to the left and get new energy
      energyUpLeft = energyUp;
      markedUpLeft = markedUp;
      energyUp = energyUpRight;
      markedUp = markedUpRight;
      if (c < numCols - 1) {
        energyUpRight = totalEnergyTo[r - 1][c + 1];
        markedUpRight = marked[r - 1][c + 1];
      }

      // assign cumulative energy to current pixel and save the column of the parent pixel
      if (minEnergyCol == -1) {
        // current pixel is unreachable from parent pixels since they are all marked
        //   OR current pixel already marked
        // set energy to reach current pixel to +INF
        totalEnergyTo[r][c] = posInf;
      }
      else {
        totalEnergyTo[r][c] = minEnergy + pixelEnergy[r][c];
      }
      colTo[r][c] = minEnergyCol;
    }
  }
}


void ct::SeamCarver::removeVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams) {
  // each row of seams stores an ordered queue of pixels to remove in that row
  //   starting with the min number column
  // each time a new column is encountered, move the pixels to the right of it
  //   (up until the next column number) to the left by the number of pixels already removed

  int32_t colToRemove = 0;
  int32_t numSeamsRemoved = 0;
  /*** REMOVE PIXELS FOR EVERY ROW ***/
  for (int32_t r = 0; r < numRows; r++) {
    // count the number of seams to the left of the current pixel
    //   to indicate how many spaces to move pixels that aren't being removed to the left
    numSeamsRemoved = 0;
    // loop through all pixels to remove in current row
    while (seams[r].size()) {
      numSeamsRemoved++;
      // column location of pixel to remove in row r
      colToRemove = seams[r].top();
      seams[r].pop();
      // mark right endpoint/next pixel column
      int32_t rightColBorder = (seams[r].empty() ? numCols : seams[r].top());
      // starting at the column to the right of the column to remove move the pixel to the left
      //   by the number of seams to the left of the pixel
      //   until the right end point which is either the last column or the next column to remove
      //   whichever comes first
      for (int c = colToRemove + 1; c < rightColBorder; c++) {
        for (int32_t j = 0; j < 3; j++) {
          bgr[j].at<uchar>(r, c - numSeamsRemoved) = bgr[j].at<uchar>(r, c);
        }
      }
    }
  }

  /*** SHRINK IMAGE ***/
  for (int i = 0; i < 3; i++) {
    bgr[i] = bgr[i].colRange(0, bgr[i].cols - numSeamsRemoved);
  }
}


void ct::SeamCarver::markVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams) {
  for (int32_t r = 0; r < numRows; r++) {
    int32_t colToRemove = 0;
    while (seams[r].size()) {
      colToRemove = seams[r].top();
      seams[r].pop();
      for (int32_t j = 0; j < 3; j++) {
        bgr[j].at<uchar>(r, colToRemove) = 0;
      }
    }
  }
}


void ct::SeamCarver::markInfEnergy(vector<cv::Mat>& bgr, vector< vector<double> >& pixelEnergy) {
  double posInf = std::numeric_limits<double>::max();
  for (int32_t r = 0; r < numRows; r++) {
    for (int32_t c = 0; c < numCols; c++) {
      if (pixelEnergy[r][c] == posInf) {
        for (int32_t j = 0; j < 3; j++) {
          bgr[j].at<uchar>(r, c) = 0;
        }
      }
    }
  }
}


void ct::SeamCarver::removeHorizontalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams) {}


void ct::SeamCarver::energy(const vector<cv::Mat>& bgr, vector< vector<double> >& outPixelEnergy) {
  int32_t bottomRow = numRows - 1;
  int32_t rightCol = numCols - 1;
  // resize output if necessary
  if (outPixelEnergy.size() != numRows) {
    outPixelEnergy.resize(numRows);
  }
  if (outPixelEnergy[0].size() != numCols) {
    for (int r = 0; r < numRows; r++) {
      outPixelEnergy[r].resize(numCols);
    }
  }
  double computedEnergy = 0.0;
  int32_t numOddCols = numCols / 2;
  int32_t numEvenCols = numCols - numOddCols;

  // locals used in energy function
  double Rx2, Rx1, Gx2, Gx1, Bx2, Bx1;
  double deltaSquareX, deltaSquareY;
  int32_t c;

  // compute energy for every row
  // do odd columns and even columns separately in order to leverage cached values to prevent multiple memory accesses
  for (int32_t r = 0; r < numRows; r++) {
    /***** ODD COLUMNS *****/
    // init starting column
    c = 1;
    // initialize color values to the left of current pixel (c = 1)
    Rx1 = bgr[2].at<uchar>(r, c - 1);
    Gx1 = bgr[1].at<uchar>(r, c - 1);
    Bx1 = bgr[0].at<uchar>(r, c - 1);
    for (int32_t n = 0; n < numOddCols; n++) {
      // return energy for border pixels
      if (r == 0 || c == 0 || r == bottomRow || c == rightCol) {
        outPixelEnergy[r][c] = this->MARGIN_ENERGY;
      }
      else {
        // compute energy for every pixel by computing gradient of colors
        // DeltaX = DeltaRx^2 + DeltaGx^2 + DeltaBx^2
        // DeltaY = DeltaRy^2 + DeltaGy^2 + DeltaBy^2
        // energy = sqrt(DeltaX + DeltaY)

        // get color values of the pixel to the right
        Rx2 = bgr[2].at<uchar>(r, c + 1);
        Gx2 = bgr[1].at<uchar>(r, c + 1);
        Bx2 = bgr[0].at<uchar>(r, c + 1);
        deltaSquareX = (pow(Rx2 - Rx1, 2.0) +  // DeltaRx^2
                        pow(Gx2 - Gx1, 2.0) +  // DeltaGx^2
                        pow(Bx2 - Bx1, 2.0));  // DeltaBx^2
        deltaSquareY = (pow(bgr[2].at<uchar>(r + 1, c) - bgr[2].at<uchar>(r - 1, c), 2.0) +  // DeltaRy^2
                        pow(bgr[1].at<uchar>(r + 1, c) - bgr[1].at<uchar>(r - 1, c), 2.0) +  // DeltaGy^2
                        pow(bgr[0].at<uchar>(r + 1, c) - bgr[0].at<uchar>(r - 1, c), 2.0));  // DeltaBy^2
        outPixelEnergy[r][c] = sqrt(deltaSquareX + deltaSquareY);

        // shift color values to the left
        Rx1 = Rx2;
        Gx1 = Gx2;
        Bx1 = Bx2;
      }
      c = c + 2;
    }

    /***** EVEN COLUMNS *****/
    // init starting column
    c = 0;
    // initialize color values to the right of current column (c = 0)
    Rx2 = bgr[2].at<uchar>(r, c + 1);
    Gx2 = bgr[1].at<uchar>(r, c + 1);
    Bx2 = bgr[0].at<uchar>(r, c + 1);
    for (int32_t n = 0; n < numEvenCols; n++) {
      // return energy for border pixels
      if (r == 0 || c == 0 || r == bottomRow || c == rightCol) {
        outPixelEnergy[r][c] = this->MARGIN_ENERGY;
      }
      else {
        // compute energy for every pixel by computing gradient of colors
        // DeltaX = DeltaRx^2 + DeltaGx^2 + DeltaBx^2
        // DeltaY = DeltaRy^2 + DeltaGy^2 + DeltaBy^2
        // energy = sqrt(DeltaX + DeltaY)

        // move color values to the left
        Rx1 = Rx2;
        Gx1 = Gx2;
        Bx1 = Bx2;
        // get new color values to the right
        Rx2 = bgr[2].at<uchar>(r, c + 1);
        Gx2 = bgr[1].at<uchar>(r, c + 1);
        Bx2 = bgr[0].at<uchar>(r, c + 1);
        deltaSquareX = (pow(Rx2 - Rx1, 2.0) +  // DeltaRx^2
                        pow(Gx2 - Gx1, 2.0) +  // DeltaGx^2
                        pow(Bx2 - Bx1, 2.0));  // DeltaBx^2
        deltaSquareY = (pow(bgr[2].at<uchar>(r + 1, c) - bgr[2].at<uchar>(r - 1, c), 2.0) +  // DeltaRy^2
                        pow(bgr[1].at<uchar>(r + 1, c) - bgr[1].at<uchar>(r - 1, c), 2.0) +  // DeltaGy^2
                        pow(bgr[0].at<uchar>(r + 1, c) - bgr[0].at<uchar>(r - 1, c), 2.0));  // DeltaBy^2
        outPixelEnergy[r][c] = sqrt(deltaSquareX + deltaSquareY);
      }
      c = c + 2;
    }
  }
}

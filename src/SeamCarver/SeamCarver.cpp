#include "SeamCarver.h"
#include "PixelEnergy2D.h"
#include <limits>
#include <chrono>
#include <thread>
using namespace std::chrono;
using std::thread;
#ifdef USEDEBUGDISPLAY
#include "DebugDisplay.h"
#endif


bool ct::KSeamCarver::FindAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn) {
  this->NumRows_ = img.size().height;
  this->NumColumns_ = img.size().width;
  this->BottomRow_ = NumRows_ - 1;
  this->RightColumn_ = NumColumns_ - 1;
  this->PosInf_ = std::numeric_limits<double>::max();

  // check if removing more seams than columns available
  if (numSeams > NumColumns_) {
    return false;
  }

  /*** DECLARE VECTORS THAT WILL BE USED THROUGHOUT THE SEAM REMOVAL PROCESS ***/
  // output of the function to compute energy
  // input to the seam finding function
  vector<vector<double>> PixelEnergy;
  // resize output if necessary
  PixelEnergy.resize(NumRows_);
  for (int r = 0; r < NumRows_; r++) {
    PixelEnergy[r].resize(NumColumns_);
  }

  // output of the seam finding function
  // input to the seam removal function
  // vector of minimum-oriented priority queues. Each row in the vector corresponds to a priority queue for that row in the image
  vecMinPQ seams;
  seams.resize(NumRows_);

  // make sure marked hasn't been set before
  // resize marked matrix to the same size as img;
  if (marked.size() != NumRows_) {
    marked.resize(NumRows_);
    for (int32_t r = 0; r < NumRows_; r++) {
      marked[r].resize(NumColumns_);
      for (int32_t c = 0; c < NumColumns_; c++) {
        marked[r][c] = false;
      }
    }
  }

  // vector to store the image's channels separately
  vector<cv::Mat> bgr;
  bgr.resize(3);

  try {
    // allocate min-oriented priority queue for each row to hold numSeams elements
    for (int32_t r = 0; r < NumRows_; r++) {
      if (!seams[r].allocate(numSeams)) {
        throw std::exception("Could not allocate memory for min oriented priority queue");
      }
    }

    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);

    // Compute pixel energy
    if (computeEnergyFn == nullptr) {
      start = high_resolution_clock::now();
      PixelEnergyCalculator_.SetDimensions(NumColumns_, NumRows_, img.channels());
      if (false == PixelEnergyCalculator_.CalculatePixelEnergy(img, PixelEnergy))
      {
        return false;
      }

      #ifdef USEDEBUGDISPLAY
      KDebugDisplay d;
      d.Display2DVector(PixelEnergy, PixelEnergyCalculator_.GetMarginEnergy());
      #endif

      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);
    }
    else {
      // call user-defined energy computation function
      computeEnergyFn(img, PixelEnergy);
    }

    // find all vertical seams
    start = high_resolution_clock::now();
    this->findVerticalSeams(numSeams, PixelEnergy, seams); // ~2.5s
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);

    // remove all found seams
    start = high_resolution_clock::now();
    //this->markVerticalSeams(bgr, seams);
    this->removeVerticalSeams(bgr, seams);  // ~55ms
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
    //this->markInfEnergy(bgr, PixelEnergy);
    //cv::merge(bgr, outImg);
    return false;
  }

  return true;
}


bool ct::KSeamCarver::findVerticalSeams(int32_t numSeams, vector< vector<double> >& pixelEnergy, vecMinPQ& outSeams) {
  if (pixelEnergy.size() == 0) {
    throw std::out_of_range("Pixel energy vector is empty\n");
  }

  if (outSeams.size() != pixelEnergy.size()) {
    throw std::out_of_range("outSeams does not have enough rows\n");
  }

  int32_t count = 0;

  // totalEnergyTo array will store cumulative energy to each pixel
  // colTo array will store the pixel's columnn in the row above to get to current pixel
  vector< vector<double> > totalEnergyTo;
  vector< vector<int32_t> > colTo;

  // resize number of rows
  totalEnergyTo.resize(NumRows_);
  colTo.resize(NumRows_);

  // resize number of columns for each row
  for (int32_t r = 0; r < NumRows_; r++) {
    totalEnergyTo[r].resize(NumColumns_);
    colTo[r].resize(NumColumns_);
  }

  // initial path calculation
  this->calculateVerticalPathEnergy(pixelEnergy, totalEnergyTo, colTo);

  // temporary seam to verify that there are no previously marked pixels in this seam
  // otherwise the cumulative energies need to be recalculated
  vector<int32_t> seam;
  seam.resize(NumRows_);

  // declare variables used in seam discovery when looking for the least cumulative energy column in the bottom row
  double minTotalEnergy = PosInf_;
  int32_t minTotalEnergyCol = -1;

  /*** RUN SEAM DISCOVERY ***/
  for (int32_t n = 0; n < numSeams; n++) {
    // find least cumulative energy column in bottom row
    // initialize total energy to +INF and run linear search for a pixel of least cumulative energy (if one exists)
    minTotalEnergy = PosInf_;
    minTotalEnergyCol = -1;
    for (int32_t c = 0; c < NumColumns_; c++) {
      if (!marked[BottomRow_][c] && totalEnergyTo[BottomRow_][c] < minTotalEnergy) {
        minTotalEnergy = totalEnergyTo[BottomRow_][c];
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
      this->calculateVerticalPathEnergy(pixelEnergy, totalEnergyTo, colTo);
      std::cout << "recalculated seam: " << n + 1 << std::endl;
      goto continueSeamFindingLoop;
    }

    // save last column as part of seam
    seam[BottomRow_] = minTotalEnergyCol;

    int32_t col = minTotalEnergyCol;
    int32_t currentCol = col;
    for (int32_t r = BottomRow_ - 1; r >= 0; r--) {
      // using the below pixel's row and column, extract the column of the pixel in the current row
      currentCol = colTo[r + 1][col];

      // check if another path of least cumulative energy can be found without recalculating energies
      if (marked[r][currentCol]) {
        // mark the starting pixel in bottom row as having +INF cumulative energy so it will not be chosen again
        totalEnergyTo[BottomRow_][minTotalEnergyCol] = PosInf_;
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
    for (int32_t r = 0; r < NumRows_; r++) {
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


void ct::KSeamCarver::calculateVerticalPathEnergy(const vector< vector<double> >& pixelEnergy, vector< vector<double> >& totalEnergyTo, vector< vector<int32_t> >& colTo) {
  // initialize top row
  for (int32_t c = 0; c < NumColumns_; c++) {
    // if previously marked, set its energy to +INF
    if (marked[0][c]) {
      totalEnergyTo[0][c] = PosInf_;
    }
    else {
      totalEnergyTo[0][c] = this->CMarginEnergy;
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
  double energyUpLeft = PosInf_;
  double energyUp = PosInf_;
  double energyUpRight = PosInf_;

  bool markedUpLeft = false;
  bool markedUp = false;
  bool markedUpRight = false;

  double minEnergy = PosInf_;
  int32_t minEnergyCol = -1;

  for (int32_t r = 1; r < NumRows_; r++) {
    energyUpLeft = PosInf_;
    energyUp = totalEnergyTo[r - 1][0];
    energyUpRight = NumColumns_ > 1 ? totalEnergyTo[r - 1][1] : PosInf_;

    markedUpLeft = true;
    markedUp = marked[r - 1][0];
    markedUpRight = NumColumns_ > 1 ? marked[r - 1][1] : true;

    // find minimum energy path from previous row to every pixel in the current row
    for (int32_t c = 0; c < NumColumns_; c++) {
      // initialize min energy to +INF and initialize the previous column to -1
      //   to set error state
      minEnergy = PosInf_;
      minEnergyCol = -1;

      // save some cycles by not doing any comparisons if the current pixel has been previously marked
      if (!marked[r][c]) {
        // check above
        if (!markedUp && energyUp < minEnergy) {
          minEnergy = energyUp;
          minEnergyCol = c;
        }

        // check if right/above is min
        if (c < NumColumns_ - 1) {
          if (!markedUpRight && energyUpRight < minEnergy) {
            minEnergy = energyUpRight;
            minEnergyCol = c + 1;
          }
        }

        // check if left/above is min
        if (c > 0) {
          if (!markedUpLeft && energyUpLeft < minEnergy) {
            minEnergy = energyUpLeft;
            minEnergyCol = c - 1;
          }
        }
      }

      // shift energy to the left
      energyUpLeft = energyUp;
      markedUpLeft = markedUp;
      energyUp = energyUpRight;
      markedUp = markedUpRight;

      // get marked and totalEnergyTo data for pixels right/above
      if (NumColumns_ > 1 && c < NumColumns_ - 2) {
        energyUpRight = totalEnergyTo[r - 1][c + 2];
        markedUpRight = marked[r - 1][c + 2];
      }

      // assign cumulative energy to current pixel and save the column of the parent pixel
      if (minEnergyCol == -1) {
        // current pixel is unreachable from parent pixels since they are all marked
        //   OR current pixel already marked
        // set energy to reach current pixel to +INF
        totalEnergyTo[r][c] = PosInf_;
      }
      else {
        totalEnergyTo[r][c] = minEnergy + pixelEnergy[r][c];
      }
      colTo[r][c] = minEnergyCol;
    }
  }
}


void ct::KSeamCarver::removeVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams) {
  // each row of seams stores an ordered queue of pixels to remove in that row
  //   starting with the min number column
  // each time a new column is encountered, move the pixels to the right of it
  //   (up until the next column number) to the left by the number of pixels already removed

  int32_t colToRemove = 0;
  int32_t numSeamsRemoved = 0;
  /*** REMOVE PIXELS FOR EVERY ROW ***/
  for (int32_t r = 0; r < NumRows_; r++) {
    // count the number of seams to the left of the current pixel
    //   to indicate how many spaces to move pixels that aren't being removed to the left
    numSeamsRemoved = 0;
    // loop through all pixels to remove in current row
    while (seams[r].size()) {
      numSeamsRemoved++;
      // column location of pixel to remove in row r
      colToRemove = seams[r].pop();
      //seams[r].pop();
      // mark right endpoint/next pixel column
      int32_t rightColBorder = (seams[r].empty() ? NumColumns_ : seams[r].top());
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

  /*** SHRINK IMAGE BY REMOVING SEAMS ***/
  for (int i = 0; i < 3; i++) {
    bgr[i] = bgr[i].colRange(0, bgr[i].cols - numSeamsRemoved);
  }
}


void ct::KSeamCarver::markVerticalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams) {
  for (int32_t r = 0; r < NumRows_; r++) {
    int32_t colToRemove = 0;
    while (seams[r].size()) {
      colToRemove = seams[r].pop();;
      //seams[r].pop();
      for (int32_t j = 0; j < 3; j++) {
        bgr[j].at<uchar>(r, colToRemove) = 0;
      }
    }
  }
}


void ct::KSeamCarver::markInfEnergy(vector<cv::Mat>& bgr, vector< vector<double> >& pixelEnergy) {
  for (int32_t r = 0; r < NumRows_; r++) {
    for (int32_t c = 0; c < NumColumns_; c++) {
      if (pixelEnergy[r][c] == PosInf_) {
        for (int32_t j = 0; j < 3; j++) {
          bgr[j].at<uchar>(r, c) = 0;
        }
      }
    }
  }
}

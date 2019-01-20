#include "SeamCarver.h"
#include <chrono>
using namespace std::chrono;
#ifdef USEDEBUGDISPLAY
#include "DebugDisplay.h"
#endif

bool ct::KSeamCarver::FindAndRemoveVerticalSeams(int32_t NumSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn) {
  this->NumRows_ = img.rows;
  this->NumColumns_ = img.cols;
  this->BottomRow_ = NumRows_ - 1;
  this->RightColumn_ = NumColumns_ - 1;
  this->PosInf_ = std::numeric_limits<double>::max();

  // check if removing more seams than columns available
  if (NumSeams > NumColumns_) {
    return false;
  }

  /*** DECLARE VECTORS THAT WILL BE USED THROUGHOUT THE SEAM REMOVAL PROCESS ***/
  // output of the function to compute energy
  // input to the CurrentSeam finding function
  vector<vector<double>> PixelEnergy;
  // resize output if necessary
  PixelEnergy.resize(NumRows_);
  for (int r = 0; r < NumRows_; r++) {
    PixelEnergy[r].resize(NumColumns_);
  }

  // output of the CurrentSeam finding function
  // input to the CurrentSeam removal function
  // vector of minimum-oriented priority queues. Each row in the vector corresponds to a priority queue for that row in the image
  VectorOfMinPQ seams;
  seams.resize(NumRows_);

  // make sure MarkedPixels hasn't been set before
  // resize MarkedPixels matrix to the same size as img;
  if (MarkedPixels.size() != NumRows_) {
    MarkedPixels.resize(NumRows_);
    for (int32_t r = 0; r < NumRows_; r++) {
      MarkedPixels[r].resize(NumColumns_);
      for (int32_t c = 0; c < NumColumns_; c++) {
        MarkedPixels[r][c] = false;
      }
    }
  }

  // vector to store the image's channels separately
  vector<cv::Mat> bgr;
  bgr.resize(3);
  cv::split(img, bgr);

  try {
    // allocate min-oriented priority queue for each row to hold NumSeams elements
    for (int32_t r = 0; r < NumRows_; r++) {
      if (!seams[r].allocate(NumSeams)) {
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
      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);

      #ifdef USEDEBUGDISPLAY
      KDebugDisplay d;
      d.Display2DVector<double>(PixelEnergy, PixelEnergyCalculator_.GetMarginEnergy());
      #endif
    }
    else {
      // TODO refactor names/parameters associated with user defined function
      // call user-defined energy computation function
      computeEnergyFn(img, PixelEnergy);
    }

    // find all vertical seams
    start = high_resolution_clock::now();
    this->FindVerticalSeams(NumSeams, PixelEnergy, seams); // ~2.5s
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);

    // remove all found seams
    start = high_resolution_clock::now();
    //this->markVerticalSeams(bgr, seams);
    this->RemoveVerticalSeams(bgr, seams);  // ~55ms
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


bool ct::KSeamCarver::FindVerticalSeams(int32_t NumSeams, vector<vector<double>>& PixelEnergy, VectorOfMinPQ& OutDiscoveredSeams) {
  if (PixelEnergy.size() == 0) {
    throw std::out_of_range("Pixel energy vector is empty\n");
  }

  if (OutDiscoveredSeams.size() != PixelEnergy.size()) {
    throw std::out_of_range("OutDiscoveredSeams does not have enough rows\n");
  }

  int32_t SeamRecalculationCount = 0;

  // TotalEnergyTo will store cumulative energy to each pixel
  // ColumnTo will store the columnn of the pixel in the row above to get to current pixel
  vector<vector<double>> TotalEnergyTo;
  vector<vector<int32_t>> ColumnTo;

  // resize number of rows
  TotalEnergyTo.resize(NumRows_);
  ColumnTo.resize(NumRows_);

  // resize number of columns for each row
  for (int32_t r = 0; r < NumRows_; r++) {
    TotalEnergyTo[r].resize(NumColumns_);
    ColumnTo[r].resize(NumColumns_);
  }

  // initial path calculation
  this->CalculateCumulativeVerticalPathEnergy(PixelEnergy, TotalEnergyTo, ColumnTo);

  // temporary CurrentSeam to verify that there are no previously MarkedPixels pixels in this CurrentSeam
  // otherwise the cumulative energies need to be recalculated
  vector<int32_t> CurrentSeam;
  CurrentSeam.resize(NumRows_);

  // declare/initialize variables used in CurrentSeam discovery when looking for the least cumulative energy column in the bottom row
  double minTotalEnergy = PosInf_;
  int32_t minTotalEnergyCol = -1;

  /*** RUN SEAM DISCOVERY ***/
  for (int32_t n = 0; n < NumSeams; n++) {
    // find least cumulative energy column in bottom row
    // initialize total energy to +INF and run linear search for a pixel of least cumulative energy (if one exists)
    minTotalEnergy = PosInf_;
    minTotalEnergyCol = -1;
    for (int32_t Column = 0; Column < NumColumns_; Column++) {
      if (!MarkedPixels[BottomRow_][Column] && TotalEnergyTo[BottomRow_][Column] < minTotalEnergy) {
        minTotalEnergy = TotalEnergyTo[BottomRow_][Column];
        minTotalEnergyCol = Column;
      }
    }

    // all pixels in bottom row are unreachable due to +INF cumulative energy to all of them
    // therefore need to recalculate cumulative energies
    if (minTotalEnergyCol == -1) {
      // decrement CurrentSeam number iterator since this CurrentSeam was invalid
      // need to recalculate the cumulative energy
      n--;
      SeamRecalculationCount++;
      this->CalculateCumulativeVerticalPathEnergy(PixelEnergy, TotalEnergyTo, ColumnTo);
      std::cout << "recalculated seam number: " << n + 1 << std::endl;
      goto ContinueSeamFindingLoop;
    }

    // save last column as part of CurrentSeam
    CurrentSeam[BottomRow_] = minTotalEnergyCol;

    int32_t col = minTotalEnergyCol;
    int32_t currentCol = col;
    for (int32_t Row = BottomRow_ - 1; Row >= 0; Row--) {
      // using the below pixel's row and column, extract the column of the pixel in the current row
      currentCol = ColumnTo[Row + 1][col];

      // check if the current seam we're swimming up has a pixel that has been used part of another seam
      if (MarkedPixels[Row][currentCol]) {
        // mark the starting pixel in bottom row as having +INF cumulative energy so it will not be chosen again
        TotalEnergyTo[BottomRow_][minTotalEnergyCol] = PosInf_;
        // decrement CurrentSeam number iterator since this CurrentSeam was invalid
        n--;
        // restart CurrentSeam finding loop
        goto ContinueSeamFindingLoop;
      }

      // save the column of the pixel in the current row
      CurrentSeam[Row] = currentCol;

      // update to current column
      col = currentCol;
    }

    // copy CurrentSeam and mark appropriate pixels
    for (int32_t Row = 0; Row < NumRows_; Row++) {
      col = CurrentSeam[Row];
      OutDiscoveredSeams[Row].push(col);
      MarkedPixels[Row][col] = true;
    }

    ContinueSeamFindingLoop: {
      continue;
    }
  }
  std::cout << "recalculated total times: " << SeamRecalculationCount << std::endl;
  return true;
}


void ct::KSeamCarver::CalculateCumulativeVerticalPathEnergy(const vector<vector<double>>& PixelEnergy, vector<vector<double>>& OutTotalEnergyTo, vector<vector<int32_t>>& OutColumnTo) {
  // initialize top row
  for (int32_t Column = 0; Column < NumColumns_; Column++) {
    // if previously MarkedPixels, set its energy to +INF
    if (MarkedPixels[0][Column]) {
      OutTotalEnergyTo[0][Column] = PosInf_;
    }
    else {
      OutTotalEnergyTo[0][Column] = this->CMarginEnergy;
    }
    OutColumnTo[0][Column] = -1;
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

  for (int32_t Row = 1; Row < NumRows_; Row++) {
    energyUpLeft = PosInf_;
    energyUp = OutTotalEnergyTo[Row - 1][0];
    energyUpRight = NumColumns_ > 1 ? OutTotalEnergyTo[Row - 1][1] : PosInf_;

    markedUpLeft = true;
    markedUp = MarkedPixels[Row - 1][0];
    markedUpRight = NumColumns_ > 1 ? MarkedPixels[Row - 1][1] : true;

    // find minimum energy path from previous row to every pixel in the current row
    for (int32_t Column = 0; Column < NumColumns_; Column++) {
      // initialize min energy to +INF and initialize the previous column to -1
      //   to set error state
      minEnergy = PosInf_;
      minEnergyCol = -1;

      // save some cycles by not doing any comparisons if the current pixel has been previously MarkedPixels
      if (!MarkedPixels[Row][Column]) {
        // check above
        if (!markedUp && energyUp < minEnergy) {
          minEnergy = energyUp;
          minEnergyCol = Column;
        }

        // check if right/above is min
        if (Column < NumColumns_ - 1) {
          if (!markedUpRight && energyUpRight < minEnergy) {
            minEnergy = energyUpRight;
            minEnergyCol = Column + 1;
          }
        }

        // check if left/above is min
        if (Column > 0) {
          if (!markedUpLeft && energyUpLeft < minEnergy) {
            minEnergy = energyUpLeft;
            minEnergyCol = Column - 1;
          }
        }
      }

      // shift energy to the left
      energyUpLeft = energyUp;
      markedUpLeft = markedUp;
      energyUp = energyUpRight;
      markedUp = markedUpRight;

      // get MarkedPixels and TotalEnergyTo data for pixels right/above
      if (NumColumns_ > 1 && Column < NumColumns_ - 2) {
        energyUpRight = OutTotalEnergyTo[Row - 1][Column + 2];
        markedUpRight = MarkedPixels[Row - 1][Column + 2];
      }

      // assign cumulative energy to current pixel and save the column of the parent pixel
      if (minEnergyCol == -1) {
        // current pixel is unreachable from parent pixels since they are all MarkedPixels
        //   OR current pixel already MarkedPixels
        // set energy to reach current pixel to +INF
        OutTotalEnergyTo[Row][Column] = PosInf_;
      }
      else {
        OutTotalEnergyTo[Row][Column] = minEnergy + PixelEnergy[Row][Column];
      }
      OutColumnTo[Row][Column] = minEnergyCol;
    }
  }
}


void ct::KSeamCarver::RemoveVerticalSeams(vector<cv::Mat>& bgr, VectorOfMinPQ& seams) {
  // each row of seams stores an ordered queue of pixels to remove in that row
  //   starting with the min number column
  // each time a new column is encountered, move the pixels to the right of it
  //   (up until the next column number) to the left by the number of pixels already removed

  int32_t colToRemove = 0;
  int32_t numSeamsRemoved = 0;
  /*** REMOVE PIXELS FOR EVERY ROW ***/
  for (int32_t r = 0; r < NumRows_; r++) {
    // SeamRecalculationCount the number of seams to the left of the current pixel
    //   to indicate how many spaces to move pixels that aren't being removed to the left
    numSeamsRemoved = 0;
    // loop through all pixels to remove in current row
    while (seams[r].size()) {
      numSeamsRemoved++;
      // column location of pixel to remove in row Row
      colToRemove = seams[r].pop();
      //seams[Row].pop();
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
  // TODO remove magic number
  int32_t NumColorChannels = 3;
  for (int32_t Channel = 0; Channel < NumColorChannels; Channel++) {
    bgr[Channel] = bgr[Channel].colRange(0, bgr[Channel].cols - numSeamsRemoved);
  }
}

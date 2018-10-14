#include "SeamCarver.h"
#include <limits>
#include <chrono>
#include <thread>
using namespace std::chrono;
using std::thread;

bool ct::SeamCarver::findAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergyFn) {
  // check if removing more seams than columns available
  if (numSeams > img.size().width) {
    return false;
  }

  int32_t numRows = img.size().height;
  int32_t numCols = img.size().width;

  /*** DECLARE VECTORS THAT WILL BE USED THROUGHOUT THE SEAM REMOVAL PROCESS ***/
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
      duration.count();

      // call built-in energy computation function
      start = high_resolution_clock::now();
      this->energy(bgr, pixelEnergy); // ~220ms
      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);
      duration.count();

    }
    else {
      // call user-defined energy computation function
      computeEnergyFn(img, pixelEnergy);
    }

    // find all vertical seams
    start = high_resolution_clock::now();
    this->findVerticalSeam(numSeams, pixelEnergy, marked, seams); // ~6.9s
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    duration.count();


    // remove all found seams
    start = high_resolution_clock::now();
    this->removeVerticalSeams(bgr, seams);  // ~65ms
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    duration.count();

    // combine separate channels into output image
    start = high_resolution_clock::now();
    cv::merge(bgr, outImg); // ~300-400us
    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    duration.count();
  }
  catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    return false;
  }

  return true;
}


bool ct::SeamCarver::findAndRemoveHorizontalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergy) {
  // check if removing more seams than rows available
  if (numSeams > img.size().height) {
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
  seams.resize(img.size().width);

  // vector to store pixels that have been previously marked for removal
  // will ignore these marked pixels when searching for a new seam
  vector< vector<bool> > marked;

  {
    int32_t imgHeight = img.size().height;
    int32_t imgWidth = img.size().width;

    // resize marked matrix to the same size as img;
    marked.resize(imgHeight);
    for (int32_t r = 0; r < imgHeight; r++) {
      marked[r].resize(imgWidth);
    }

    // initialize marked matrix to false;
    for (int32_t r = 0; r < imgHeight; r++) {
      for (int32_t c = 0; c < imgWidth; c++) {
        marked[r][c] = false;
      }
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

    // find all horizontal seams
    for (int32_t i = 0; i < numSeams; i++) {
      this->findHorizontalSeam(pixelEnergy, marked, seams);
    }

    // remove all found seams
    this->removeHorizontalSeams(bgr, seams);

    // combine separate channels into output image
    cv::merge(bgr, outImg);
  }
  catch (std::out_of_range e) {
    std::cout << e.what() << std::endl;
    return false;
  }

  return true;
}


bool ct::SeamCarver::findVerticalSeam(int32_t numSeams, const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeams) {
  if (pixelEnergy.size() == 0) {
    throw std::out_of_range("Pixel energy vector is empty\n");
  }

  if (outSeams.size() != pixelEnergy.size()) {
    throw std::out_of_range("outSeams does not have enough rows\n");
  }

  // initialize constants to be used throughout function
  int32_t numRows = pixelEnergy.size();
  int32_t numCols = pixelEnergy[0].size();
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
  auto start = high_resolution_clock::now();
  this->calculateVerticalPathEnergy(pixelEnergy, marked, totalEnergyTo, colTo); // ~320-335us
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);

  // temporary seam to verify that there are no previously marked pixels in this seam
  // otherwise the cumulative energies need to be recalculated
  vector<int32_t> seam;
  seam.resize(numRows);

  // declare variables used in seam discovery when looking for the least cumulative energy column in the bottom row
  double minTotalEnergy = posInf;
  int32_t minTotalEnergyCol = -1;

  /*** RUN numSeams SEAM DISCOVERY ***/
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

    // just as a precaution if for some reason all pixels in bottom row have been marked
    if (minTotalEnergyCol == -1) {
      return false;
    }

    // save last column as part of seam
    seam[bottomRow] = minTotalEnergyCol;

    int32_t col = minTotalEnergyCol;
    int32_t currentCol = col;
    for (int32_t r = bottomRow - 1; r >= 0; r--) {
      // using the below pixel's row and column, extract the column of the pixel in the current row
      currentCol = colTo[r + 1][col];

      // check if path energies need to be recalculated since the path has been marked by another path
      if (marked[r][currentCol]) {
        // break inner loop, recalculate energy path and restart finding seam number n
        goto recalculateVerticalEnergy;
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

    continue;

    recalculateVerticalEnergy: {
    // decrement seam number iterator to restart seam path discovery for this seam
    // need to recalculate the cumulative energy
    n--;
    this->calculateVerticalPathEnergy(pixelEnergy, marked, totalEnergyTo, colTo);
    }
  }
  return true;
}


void ct::SeamCarver::findHorizontalSeam(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeams) {}


void ct::SeamCarver::calculateVerticalPathEnergy(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vector< vector<double> >& totalEnergyTo, vector< vector<int32_t> >& colTo) {
  // initialize constants to be used throughout function
  int32_t numRows = pixelEnergy.size();
  int32_t numCols = pixelEnergy[0].size();
  double posInf = std::numeric_limits<double>::max();

  // initialize top row
  for (int32_t c = 0; c < numCols; c++) {
    // if previously marked, set its energy to +INF
    if (marked[0][c]) {
      totalEnergyTo[0][c] = posInf;
    }
    else {
      totalEnergyTo[0][c] = pixelEnergy[0][c];
    }
    colTo[0][c] = -1;
  }

  // cache the total energy to the pixels up/left, directly above, and up/right
  //   instead of accessing memory for the same pixels
  // all that needs to be done, is shifting energies to the left and accessing one memory location
  // left/above = directly above
  // directly above = right/above
  // right/above = access new memory
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
    energyUpRight = numCols > 2 ? totalEnergyTo[r - 1][1] : posInf;

    markedUpLeft = true;
    markedUp = marked[r - 1][0];
    markedUpRight = numCols > 2 ? marked[r - 1][1] : true;

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

        // check if left/above is min
        if (c > 0) {
          if (!markedUpLeft && energyUpLeft < minEnergy) {
            minEnergy = totalEnergyTo[r - 1][c - 1];
            minEnergyCol = c - 1;
          }
        }

        // check if right/above is min
        if (c < numCols - 1) {
          if (!markedUpRight && energyUpRight < minEnergy) {
            minEnergy = totalEnergyTo[r - 1][c + 1];
            minEnergyCol = c + 1;
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
  uint32_t numRows = bgr[0].size().height;
  uint32_t numCols = bgr[0].size().width;
  for (uint32_t r = 0; r < numRows; r++) {
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
        for (int j = 0; j < 3; j++) {
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


void ct::SeamCarver::removeHorizontalSeams(vector<cv::Mat>& bgr, vecMinPQ& seams) {
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
  int32_t numRows = bgr[0].size().height;
  int32_t numCols = bgr[0].size().width;
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
  for (int32_t r = 0; r < numRows; r++) {
    for (int32_t c = 0; c < numCols; c++) {
      if (this->energyAt(bgr, r, c, computedEnergy)) {
      }
      outPixelEnergy[r][c] = computedEnergy;
    }
  }
}

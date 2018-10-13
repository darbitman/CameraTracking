#include "SeamCarver.h"
#include <limits>
#include <chrono>
using namespace std::chrono;


bool ct::SeamCarver::findAndRemoveVerticalSeams(int32_t numSeams, const cv::Mat& img, cv::Mat& outImg, ct::energyFunc computeEnergy) {
  // check if removing more seams than columns available
  if (numSeams > img.size().width) {
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
  seams.resize(img.size().height);

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
    auto start = high_resolution_clock::now();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    // compute energy of pixels
    if (computeEnergy == nullptr) {
      // split img into 3 channels (BLUE, GREEN, RED)
      start = high_resolution_clock::now();
      cv::split(outImg, bgr); // 300-400us
      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);
      duration.count();

      // call built-in energy computation function
      start = high_resolution_clock::now();
      this->energy(bgr, pixelEnergy); // ~200-250ms
      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);
      duration.count();
      
    }
    else {
      // call user-defined energy computation function
      computeEnergy(outImg, pixelEnergy);
    }

    // find all vertical seams
    
    for (int32_t i = 0; i < numSeams; i++) {  // ~16-17sec
      start = high_resolution_clock::now();
      this->findVerticalSeam(pixelEnergy, marked, seams); // 300-400ms
      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);
      duration.count();
    }

    
    // remove all found seams
    start = high_resolution_clock::now();
    this->removeVerticalSeams(bgr, seams);  // ~60-70ms
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


bool ct::SeamCarver::findVerticalSeam(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeams) {
  if (pixelEnergy.size() == 0) {
    throw std::out_of_range("Pixel energy vector is empty\n");
  }

  if (outSeams.size() != pixelEnergy.size()) {
    throw std::out_of_range("outSeams does not have enough rows\n");
  }

  // totalEnergyTo array will store cumulative energy to each pixel
  // colTo array will store the pixel's columnn in the row above to get to current pixel
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
  outSeams[bottomRow].push(minTotalEnergyCol);
  
  // mark the column of the pixel in the bottom row
  marked[bottomRow][minTotalEnergyCol] = true;

  int32_t col = minTotalEnergyCol;
  // trace path upwards
  for (int32_t r = bottomRow - 1; r >= 0; r--) {
    // using the below pixel's row and column, extract the column of the pixel in the current row
    //   that will be removed as part of the "min" energy seam
    // save the column of the pixel in the current row
    outSeams[r].push(colTo[r + 1][col]);

    // update to current column
    col = colTo[r + 1][col];

    // mark the column of the pixel in the current row
    marked[r][col] = true;
  }

  return true;
}


void ct::SeamCarver::findHorizontalSeam(const vector< vector<double> >& pixelEnergy, vector < vector<bool> >& marked, vecMinPQ& outSeams) {
  if (pixelEnergy.size() == 0) {
    throw std::out_of_range("Pixel energy vector is empty\n");
  }

  if (outSeams.size() != pixelEnergy.size()) {
    throw std::out_of_range("outSeams does not have enough rows\n");
  }

  // totalEnergyTo array will store cumulative energy to each pixel
  // rowTo array will store the pixel's columnn in the row above to get to current pixel
  vector< vector<double> > totalEnergyTo;
  vector< vector<int32_t> > rowTo;
  
  {
    int32_t numRows = pixelEnergy.size();
    int32_t numCols = pixelEnergy[0].size();

    // resize number of rows
    totalEnergyTo.resize(numRows);
    rowTo.resize(numRows);

    // resize number of columsn for each row
    for (int32_t r = 0; r < numRows; r++) {
      totalEnergyTo[r].resize(numCols);
      rowTo[r].resize(numCols);
    }
  }

  // initialize left most column
  for (int32_t r = 0; r < totalEnergyTo.size(); r++) {
    // if previously marked, set its energy to +INF
    if (marked[r][0]) {
      totalEnergyTo[r][0] = std::numeric_limits<double>::max();
    }
    else {
      totalEnergyTo[r][0] = pixelEnergy[r][0];
    }
    rowTo[r][0] = -1;
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
  for (uint32_t r = 0; r < seams.size(); r++) {
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
      int32_t rightColBorder = (seams[r].size() ? seams[r].top() : bgr[0].size().width);
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
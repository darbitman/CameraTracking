#include "PixelEnergy2D.h"

ct::KPixelEnergy2D::KPixelEnergy2D(double MarginEnergy)
{
  MarginEnergy_ = MarginEnergy;
}

ct::KPixelEnergy2D::KPixelEnergy2D(int32_t NumColumns, int32_t NumRows, int32_t NumChannels, double MarginEnergy)
{
  ImageDimensions.NumColumns_ = NumColumns;
  ImageDimensions.NumRows_ = NumRows;
  ImageDimensions.NumColorChannels_ = NumChannels;
  MarginEnergy_ = MarginEnergy;
  bDimensionsInitialized = true;
}

ct::KPixelEnergy2D::KPixelEnergy2D(const cv::Mat& Image, double MarginEnergy)
{
  ImageDimensions.NumColumns_ = Image.cols;
  ImageDimensions.NumRows_ = Image.rows;
  ImageDimensions.NumColorChannels_ = Image.channels();
  MarginEnergy_ = MarginEnergy;
  bDimensionsInitialized = true;
}

double ct::KPixelEnergy2D::GetMarginEnergy() const
{
  return MarginEnergy_;
}

void ct::KPixelEnergy2D::SetMarginEnergy(double MarginEnergy)
{
  MarginEnergy_ = MarginEnergy;
}

bool ct::KPixelEnergy2D::GetDimensions(ImageDimensionStruct& OutImageDimensions) const
{
  if (!bDimensionsInitialized) {
    return false;
  }
  else
  {
    OutImageDimensions.NumColumns_ = ImageDimensions.NumColumns_;
    OutImageDimensions.NumRows_ = ImageDimensions.NumRows_;
    OutImageDimensions.NumColorChannels_ = ImageDimensions.NumColorChannels_;
    return true;
  }
}

void ct::KPixelEnergy2D::SetDimensions(int32_t NumColumns, int32_t NumRows, int32_t NumChannels)
{
  ImageDimensions.NumColumns_ = NumColumns;
  ImageDimensions.NumRows_ = NumRows;
  ImageDimensions.NumColorChannels_ = NumChannels;
}

bool ct::KPixelEnergy2D::CalculatePixelEnergyForEveryRow(const cv::Mat& Image, vector<vector<double>>& OutPixelEnergy, bool bDoOddColumns)
{
  // ensure OutPixelEnergy is of the right size
  if (Image.cols != ImageDimensions.NumColumns_ &&
      Image.rows != ImageDimensions.NumRows_ &&
      Image.channels() != ImageDimensions.NumColorChannels_)
  {
    return false;
  }

  // ensure OutPixelEnergy has the right dimensions
  // if not, then resize locally
  if (!(OutPixelEnergy.size() == ImageDimensions.NumRows_) || !(OutPixelEnergy[0].size() == ImageDimensions.NumColumns_))
  {
    OutPixelEnergy.resize(ImageDimensions.NumRows_);
    for (int32_t Row = 0; Row < ImageDimensions.NumRows_; Row++)
    {
      OutPixelEnergy[Row].resize(ImageDimensions.NumColumns_);
    }
  }

  // TODO can these local variables be moved higher into the private section of the class
  int32_t BottomRow = ImageDimensions.NumRows_ - 1;
  int32_t RightColumn = ImageDimensions.NumColumns_ - 1;

  // Ensure Image has non-zero dimensions
  if (Image.cols == 0 || Image.rows == 0 || Image.channels() == 0) { return false; }

  // Verify that image dimensions match the local ones
  if (Image.cols != ImageDimensions.NumColumns_ ||
      Image.rows != ImageDimensions.NumRows_ ||
      Image.channels() != ImageDimensions.NumColorChannels_)
  {
    return false;
  }
  vector<cv::Mat> ImageByChannel;
  ImageByChannel.resize(ImageDimensions.NumColorChannels_);

  // if color channels use cv::split
  // otherwise if grayscale use cv::extractChannel
  // TODO compute depending on the number of channels
  if (ImageDimensions.NumColorChannels_ == CNumChannelsInColorImage_)
  {
    cv::split(Image, ImageByChannel);
  }
  else
  {
    cv::extractChannel(Image, ImageByChannel[0], 0);
  }

  // Establish vectors whose size is equal to the number of channels
  // Two vectors used to compute X gradient
    // Don't need them for Y since we are only caching the columns
    // We can just access the pixel values above/below directly to compute the delta
  // TODO replace vectors with a multidimensional vector
  vector<double> XDirection2;
  vector<double> XDirection1;

  XDirection2.resize(ImageDimensions.NumColorChannels_);
  XDirection1.resize(ImageDimensions.NumColorChannels_);

  vector<double> DeltaXDirection;
  vector<double> DeltaYDirection;

  DeltaXDirection.resize(ImageDimensions.NumColorChannels_);
  DeltaYDirection.resize(ImageDimensions.NumColorChannels_);

  double DeltaSquareX = 0.0;
  double DeltaSquareY = 0.0;

  int32_t Column = 0;
  // compute energy for every row
  // do odd columns and even columns separately in order to leverage cached values to prevent multiple memory accesses
  for (int32_t Row = 0; Row < ImageDimensions.NumRows_; Row++)
  {
    /***** ODD COLUMNS *****/
    if (bDoOddColumns)
    {
      // initialize starting column
      Column = 1;
      
      // initialize color values to the left of current pixel
      for (int32_t Channel = 0; Channel < ImageDimensions.NumColorChannels_; Channel++)
      {
        XDirection1[Channel] = ImageByChannel[Channel].at<uchar>(Row, Column - 1);
      }

      // Compute energy of odd columns
      for (/* Column was already initialized */; Column < ImageDimensions.NumColumns_; Column += 2)
      {
        if (Row == 0 || Column == 0 || Row == BottomRow || Column == RightColumn)
        {
          OutPixelEnergy[Row][Column] = MarginEnergy_;
        }
        else
        {
          // Reset gradient from previous calculation
          DeltaSquareX = 0.0;
          DeltaSquareY = 0.0;

          // For all channels:
            // Compute gradients
            // Compute overall energy by summing both X and Y gradient
          for (int32_t Channel = 0; Channel < ImageDimensions.NumColorChannels_; Channel++)
          {
            // get new values to the right
            XDirection2[Channel] = ImageByChannel[Channel].at<uchar>(Row, Column + 1);

            DeltaXDirection[Channel] = XDirection2[Channel] - XDirection1[Channel];

            DeltaSquareX += DeltaXDirection[Channel] * DeltaXDirection[Channel];

            DeltaYDirection[Channel] =
              ImageByChannel[Channel].at<uchar>(Row + 1, Column) -
              ImageByChannel[Channel].at<uchar>(Row - 1, Column);

            DeltaSquareY += DeltaYDirection[Channel] * DeltaYDirection[Channel];

            // shift color values to the left
            XDirection1[Channel] = XDirection2[Channel];
          }
          OutPixelEnergy[Row][Column] = DeltaSquareX + DeltaSquareY;
        }
      }
    }
    /***** EVEN COLUMNS *****/
    else
    {
      // initialize starting column
      Column = 0;

      // initialize color values to the right of current pixel
      for (int32_t Channel = 0; Channel < ImageDimensions.NumColorChannels_; Channel++)
      {
        XDirection2[Channel] = ImageByChannel[Channel].at<uchar>(Row, Column + 1);
      }

      // Compute energy of odd columns
      for (/* Column was already initialized */; Column < ImageDimensions.NumColumns_; Column += 2)
      {
        if (Row == 0 || Column == 0 || Row == BottomRow || Column == RightColumn)
        {
          OutPixelEnergy[Row][Column] = MarginEnergy_;
        }
        else
        {
          // Reset gradient from previous calculation
          DeltaSquareX = 0.0;
          DeltaSquareY = 0.0;

          // For all channels:
            // Compute gradients
            // Compute overall energy by summing both X and Y gradient
          for (int32_t Channel = 0; Channel < ImageDimensions.NumColorChannels_; Channel++)
          {
            // shift color values to the left
            XDirection1[Channel] = XDirection2[Channel];

            // get new values to the right
            XDirection2[Channel] = ImageByChannel[Channel].at<uchar>(Row, Column + 1);
            DeltaXDirection[Channel] = XDirection2[Channel] - XDirection1[Channel];

            DeltaSquareX += DeltaXDirection[Channel] * DeltaXDirection[Channel];

            DeltaYDirection[Channel] =
              ImageByChannel[Channel].at<uchar>(Row + 1, Column) -
              ImageByChannel[Channel].at<uchar>(Row - 1, Column);

            DeltaSquareY += DeltaYDirection[Channel] * DeltaYDirection[Channel];
          }
          OutPixelEnergy[Row][Column] = DeltaSquareX + DeltaSquareY;
        }
      }
    }
  }
  return true;
}

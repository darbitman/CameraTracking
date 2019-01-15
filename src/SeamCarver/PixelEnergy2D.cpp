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

bool ct::KPixelEnergy2D::CalculatePixelEnergy(const cv::Mat& Image, vector<vector<double>>& OutPixelEnergy)
{
  // Ensure Image has non-zero dimensions
  if (Image.cols == 0 || Image.rows == 0 || Image.channels == 0) { return false; }

  // Verify that image dimensions match the local ones
  if (Image.cols != ImageDimensions.NumColumns_ ||
      Image.rows != ImageDimensions.NumRows_ ||
      Image.channels != ImageDimensions.NumColorChannels_)
  {
    return false;
  }
  vector<cv::Mat> ImageByChannel;
  ImageByChannel.resize(ImageDimensions.NumColorChannels_);

  // if color channels use cv::split
  // otherwise if grayscale use cv::extractChannel
  if (ImageDimensions.NumColorChannels_ == CNumChannelsInColorImage_)
  {
    cv::split(Image, ImageByChannel);
  }
  else
  {
    cv::extractChannel(Image, ImageByChannel[0], 0);
  }
  return false;
}

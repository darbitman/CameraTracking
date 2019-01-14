#include "PixelEnergy2D.h"

ct::KPixelEnergy2D::KPixelEnergy2D(double MarginEnergy)
{
  MarginEnergy_ = MarginEnergy;
}

ct::KPixelEnergy2D::KPixelEnergy2D(int32_t NumColumns, int32_t NumRows, double MarginEnergy)
{
  NumColumns_ = NumColumns;
  NumRows_ = NumRows;
  MarginEnergy_ = MarginEnergy;
  bDimensionsInitialized = true;
}

ct::KPixelEnergy2D::KPixelEnergy2D(const cv::Mat& PixelData, double MarginEnergy)
{
  NumColumns_ = PixelData.cols;
  NumRows_ = PixelData.rows;
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

bool ct::KPixelEnergy2D::GetDimensions(int32_t& OutNumCols, int32_t& OutNumRows) const
{
  if (!bDimensionsInitialized) {
    return false;
  }
  else
  {
    OutNumCols = NumColumns_;
    OutNumRows = NumRows_;
    return true;
  }
}

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

void ct::KPixelEnergy2D::GetDimensions(ImageDimensionStruct& OutImageDimensions) const
{
    if (!bDimensionsInitialized)
    {
        throw std::logic_error("Attempting to access uninitialized dimensions\n");
    }
    else
    {
        OutImageDimensions.NumColumns_ = ImageDimensions.NumColumns_;
        OutImageDimensions.NumRows_ = ImageDimensions.NumRows_;
        OutImageDimensions.NumColorChannels_ = ImageDimensions.NumColorChannels_;
    }
}

void ct::KPixelEnergy2D::SetDimensions(int32_t NumColumns, int32_t NumRows, int32_t NumChannels)
{
    ImageDimensions.NumColumns_ = NumColumns;
    ImageDimensions.NumRows_ = NumRows;
    ImageDimensions.NumColorChannels_ = NumChannels;
}

bool ct::KPixelEnergy2D::CalculatePixelEnergy(const cv::Mat & Image, vector<vector<double>>& OutPixelEnergy)
{
    // TODO add threads
      // if more columns, split calculation into 2 threads to calculate for every row
    if (ImageDimensions.NumColumns_ >= ImageDimensions.NumRows_)
    {
        return CalculatePixelEnergyForEveryRow(Image, OutPixelEnergy, true) &&
            CalculatePixelEnergyForEveryRow(Image, OutPixelEnergy, false);
    }
    // otherwise, if more rows, split calculation into 2 threads to calculate for every column
    else
    {
        return CalculatePixelEnergyForEveryColumn(Image, OutPixelEnergy, true) &&
            CalculatePixelEnergyForEveryColumn(Image, OutPixelEnergy, false);
    }

    return false;
}

bool ct::KPixelEnergy2D::CalculatePixelEnergyForEveryRow(const cv::Mat& Image, vector<vector<double>>& OutPixelEnergy, bool bDoOddColumns)
{
    // ensure Image is of the right size
    if (!(Image.cols == ImageDimensions.NumColumns_ &&
          Image.rows == ImageDimensions.NumRows_ &&
          Image.channels() == ImageDimensions.NumColorChannels_))
    {
        return false;
    }

    // ensure Image has non-zero dimensions
    if (Image.cols == 0 || Image.rows == 0 || Image.channels() == 0) { return false; }

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

    vector<cv::Mat> ImageByChannel;
    ImageByChannel.resize(ImageDimensions.NumColorChannels_);

    // if color channels use cv::split
    // otherwise if grayscale use cv::extractChannel
    if (ImageDimensions.NumColorChannels_ == CNumChannelsInColorImage_)
    {
        cv::split(Image, ImageByChannel);
    }
    else if (ImageDimensions.NumColorChannels_ == 1)
    {
        cv::extractChannel(Image, ImageByChannel[0], 0);
    }
    else
    {
        throw std::domain_error("Number of channels is not 3 (color) nor 1 (grayscale)");
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
                    // Reset gradients from previous calculation
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

bool ct::KPixelEnergy2D::CalculatePixelEnergyForEveryColumn(const cv::Mat& Image, vector< vector<double> >& OutPixelEnergy, bool bDoOddRows)
{
    // ensure Image is of the right size
    if (!(Image.cols == ImageDimensions.NumColumns_ &&
          Image.rows == ImageDimensions.NumRows_ &&
          Image.channels() == ImageDimensions.NumColorChannels_))
    {
        return false;
    }

    // ensure Image has non-zero dimensions
    if (Image.cols == 0 || Image.rows == 0 || Image.channels() == 0) { return false; }

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
    vector<double> YDirection2;
    vector<double> YDirection1;

    YDirection2.resize(ImageDimensions.NumColorChannels_);
    YDirection1.resize(ImageDimensions.NumColorChannels_);

    vector<double> DeltaXDirection;
    vector<double> DeltaYDirection;

    DeltaXDirection.resize(ImageDimensions.NumColorChannels_);
    DeltaYDirection.resize(ImageDimensions.NumColorChannels_);

    double DeltaSquareX = 0.0;
    double DeltaSquareY = 0.0;

    int32_t Row = 0;
    // compute energy for every column
    // do odd rows and even rows separately in order to leverage cached values to prevent multiple memory accesses
    for (int32_t Column = 0; Column < ImageDimensions.NumColumns_; Column++)
    {
        /***** ODD ROWS *****/
        if (bDoOddRows)
        {
            // initialize starting row
            Row = 1;

            // initialize color values above the current pixel
            for (int32_t Channel = 0; Channel < ImageDimensions.NumColorChannels_; Channel++)
            {
                YDirection1[Channel] = ImageByChannel[Channel].at<uchar>(Row - 1, Column);
            }

            // Compute energy of odd rows
            for (/* Row was already initialized */; Row < ImageDimensions.NumRows_; Row += 2)
            {
                if (Row == 0 || Column == 0 || Row == BottomRow || Column == RightColumn)
                {
                    OutPixelEnergy[Row][Column] = MarginEnergy_;
                }
                else
                {
                    // Reset gradients from previous calculation
                    DeltaSquareX = 0.0;
                    DeltaSquareY = 0.0;

                    // For all channels:
                      // Compute gradients
                      // Compute overall energy by summing both X and Y gradient
                    for (int32_t Channel = 0; Channel < ImageDimensions.NumColorChannels_; Channel++)
                    {
                        // get new values below the current pixel
                        YDirection2[Channel] = ImageByChannel[Channel].at<uchar>(Row + 1, Column);

                        DeltaYDirection[Channel] = YDirection2[Channel] - YDirection1[Channel];

                        DeltaSquareY = DeltaYDirection[Channel] * DeltaYDirection[Channel];

                        DeltaXDirection[Channel] =
                            ImageByChannel[Channel].at<uchar>(Row, Column + 1) -
                            ImageByChannel[Channel].at<uchar>(Row, Column - 1);

                        DeltaSquareX += DeltaXDirection[Channel] * DeltaXDirection[Channel];

                        // shift color values up
                        YDirection1[Channel] = YDirection2[Channel];
                    }
                    OutPixelEnergy[Row][Column] = DeltaSquareX + DeltaSquareY;
                }
            }
        }
        /***** EVEN ROWS *****/
        else
        {
            // initialize starting row
            Row = 0;

            // initialize color values below the current pixel
            for (int32_t Channel = 0; Channel < ImageDimensions.NumColorChannels_; Channel++)
            {
                YDirection2[Channel] = ImageByChannel[Channel].at<uchar>(Row + 1, Column);
            }

            // Compute energy of odd rows
            for (/* Row was already initialized */; Row < ImageDimensions.NumRows_; Row += 2)
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
                        // shift color values up
                        YDirection1[Channel] = YDirection2[Channel];

                        // get new values below the current pixel
                        YDirection2[Channel] = ImageByChannel[Channel].at<uchar>(Row + 1, Column);
                        DeltaYDirection[Channel] = YDirection2[Channel] - YDirection1[Channel];

                        DeltaSquareY += DeltaYDirection[Channel] * DeltaYDirection[Channel];

                        DeltaXDirection[Channel] =
                            ImageByChannel[Channel].at<uchar>(Row, Column + 1) -
                            ImageByChannel[Channel].at<uchar>(Row, Column - 1);

                        DeltaSquareX += DeltaXDirection[Channel] * DeltaXDirection[Channel];
                    }
                    OutPixelEnergy[Row][Column] = DeltaSquareX + DeltaSquareY;
                }
            }
        }
    }
    return true;
}
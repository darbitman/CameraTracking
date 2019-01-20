#pragma once
#include <vector>
#include <opencv2/opencv.hpp>

using std::vector;

class KDebugDisplay
{
public:
    KDebugDisplay();

    /**
     * @brief Converts a 2D vector of ValueType (e.g. double) to a grayscale cv::Mat internally and
            displays it
     * @param InputVector: grayscale representation of an image
     * @param NormalizationFactor: Factor by which to normalize the values such that any value is
            no larger than 1.0
     */
    template<typename ValueType>
    bool Display2DVector(const vector<vector<ValueType>>& InputVector,
                         ValueType NormalizationFactor);

    /**
     * @brief
     * @param
     * @param
     * @param
     */
    bool MarkPixelsAndDisplay(const vector<vector<bool>>& PixelsToMark,
                              const cv::Mat& ImageToMark,
                              uchar Color = 255);

private:
    void WaitForEscKey() const;
};


KDebugDisplay::KDebugDisplay() {}

template<typename ValueType>
bool KDebugDisplay::Display2DVector(const vector<vector<ValueType>>& InputVector,
                                    ValueType NormalizationFactor)
{
    if (!(InputVector.size() > 0 && InputVector[0].size() > 0)) { return false; }

    // create an output matrix of the same dimensions as input
    cv::Mat output(InputVector.size(), InputVector[0].size(), CV_8UC1);

    for (uint32_t Row = 0; Row < InputVector.size(); Row++)
    {
        for (uint32_t Column = 0; Column < InputVector[0].size(); Column++)
        {
            output.at<uchar>(Row, Column) = (uchar)(InputVector[Row][Column] /
                                                    NormalizationFactor * ((2 << ((sizeof(uchar) * 8) - 1)) - 1));
        }
    }

    cv::namedWindow("DebugDisplay - Display2DVector");
    cv::imshow("DebugDisplay - Display2DVector", output);

    WaitForEscKey();
    return true;
}

bool KDebugDisplay::MarkPixelsAndDisplay(const vector<vector<bool>>& PixelsToMark,
                                         const cv::Mat& ImageToMark, uchar Color)
{
    int32_t NumChannels = ImageToMark.channels();
    int32_t NumColumns = ImageToMark.cols;
    int32_t NumRows = ImageToMark.rows;

    if (!(PixelsToMark.size() == NumRows && PixelsToMark[0].size() == NumColumns))
    {
        return false;
    }

    vector<cv::Mat> ImageByChannel;
    ImageByChannel.resize(NumChannels);

    // split the cv::Mat into separate channels
    if (NumChannels == 3)
    {
        cv::split(ImageToMark, ImageByChannel);
    }
    else if (NumChannels == 1)
    {
        cv::extractChannel(ImageToMark, ImageByChannel[0], 0);
    }
    else
    {
        return false;
    }

    for (int32_t Row = 0; Row < NumRows; Row++)
    {
        for (int32_t Column = 0; Column < NumColumns; Column++)
        {
            if (PixelsToMark[Row][Column])
            {
                for (int32_t Channel = 0; Channel < NumChannels; Channel++)
                {
                    ImageByChannel[Channel].at<uchar>(Row, Column) = Color;
                }
            }
        }
    }
    cv::Mat output;
    cv::merge(ImageByChannel, output);
    cv::namedWindow("DebugDisplay - MarkPixels");
    cv::imshow("DebugDisplay - MarkPixels", output);
    WaitForEscKey();
    return true;
}

void KDebugDisplay::WaitForEscKey() const
{
    while (true)
    {
        if (cv::waitKey(50) == 27)
        {
            break;
        }
    }
}

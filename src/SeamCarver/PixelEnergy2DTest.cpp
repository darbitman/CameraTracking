#include "PixelEnergy2D.h"
#include "gtest/gtest.h"

#ifdef USEDEBUGDISPLAY
#include "DebugDisplay.h"
#endif


TEST(PixelEnergy2D, CTORGettersAndSetters)
{
    cv::Mat img = cv::imread("../../../images/guitar.png");
    ASSERT_EQ(img.empty(), false);

    ct::KPixelEnergy2D PixelEnergyCalculator(img);
    ImageDimensionStruct  ImageDimensions;

    EXPECT_EQ(PixelEnergyCalculator.GetDimensions(ImageDimensions), true);
    EXPECT_EQ(ImageDimensions.NumColorChannels_, img.channels());
    EXPECT_EQ(ImageDimensions.NumColumns_, img.cols);
    EXPECT_EQ(ImageDimensions.NumRows_, img.rows);
    EXPECT_EQ(PixelEnergyCalculator.GetMarginEnergy(), 390150.0);

    double NewMarginEnergy = 400000.0;
    PixelEnergyCalculator.SetMarginEnergy(NewMarginEnergy);

    EXPECT_EQ(PixelEnergyCalculator.GetMarginEnergy(), NewMarginEnergy);

    int32_t NewNumRows = 100;
    int32_t NewNumColumns = 200;
    int32_t NewNumChannels = 1;

    PixelEnergyCalculator.SetDimensions(NewNumColumns, NewNumRows, NewNumChannels);

    EXPECT_EQ(PixelEnergyCalculator.GetDimensions(ImageDimensions), true);
    EXPECT_EQ(ImageDimensions.NumColorChannels_, NewNumChannels);
    EXPECT_EQ(ImageDimensions.NumColumns_, NewNumColumns);
    EXPECT_EQ(ImageDimensions.NumRows_, NewNumRows);

    vector<vector<double>> ComputedPixelEnergy;
    EXPECT_EQ(PixelEnergyCalculator.CalculatePixelEnergy(img, ComputedPixelEnergy), false);

    PixelEnergyCalculator.SetDimensions(img.cols, img.rows, img.channels());
    EXPECT_EQ(PixelEnergyCalculator.GetDimensions(ImageDimensions), true);
    EXPECT_EQ(ImageDimensions.NumColorChannels_, img.channels());
    EXPECT_EQ(ImageDimensions.NumColumns_, img.cols);
    EXPECT_EQ(ImageDimensions.NumRows_, img.rows);

    EXPECT_EQ(PixelEnergyCalculator.CalculatePixelEnergy(img, ComputedPixelEnergy), true);


#ifdef USEDEBUGDISPLAY
    KDebugDisplay d;
    d.Display2DVector<double>(ComputedPixelEnergy, PixelEnergyCalculator.GetMarginEnergy());
    vector<vector<bool>> Marked;
    Marked.resize(img.rows);
    for (int32_t Row = 0; Row < img.rows; Row++)
    {
        Marked[Row].resize(img.cols);
    }
    for (int32_t Row = 0; Row < img.rows; Row++)
    {
        Marked[Row][img.cols / 2] = true;
    }
    d.MarkPixelsAndDisplay(Marked, img, 100);
#endif
}

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    return 0;
}

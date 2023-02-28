
#include "BackProjection.hpp"
#include "utils.hpp"

/**
 * @brief Compute histogram backprojection map segmentation from image and mask
 * 
 * @param image where the histogram and backprojection is computed
 * @param mask where to compute the histogram
 * @return Mat Binary image where white pixel are likely to belong to the estimated distribution
 */
Mat histogramBackProject(const Mat &image, Mat mask)
{

    // return;

    Mat res = image.clone();
    Mat colorSpaced;
    // cvtColor(image, colorSpaced, COLOR_BGR2YCrCb);

    if (mask.cols != image.cols || mask.rows != image.rows)
    {
        resize(mask, mask, {image.cols, image.rows}, 0, 0, INTER_LINEAR);
    }

    mask = mask & 0x01; // select both GC_FGD    = 1 and  GC_PR_FGD = 3

    int binSize = 32;

    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 2}; // b g r

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int c0Bins = binSize, c1Bins = binSize, c2Bins = binSize;
    int histSize[] = {c0Bins, c1Bins};

    float c0Ranges[] = {0, 255}; // hue varies from 0 to 179, see cvtColor
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float c1Ranges[] = {0, 255};
    float c2Ranges[] = {0, 255};
    const float *ranges[] = {c0Ranges, c1Ranges};
    Mat hist;

    calcHist(&image, 1, channels, mask,
             hist, 2, histSize, ranges,
             true, // the histogram is uniform
             false);

    // normalize histogram 0 - 255
    hist = (hist * 255) / sum(hist);

    // cout << sum(hist) << " " << endl;

    Mat histVisu;
    resize(hist, histVisu, {256, 256}, 0, 0, INTER_NEAREST);
    imshowDebug("hist", histVisu * 255);

    Mat backproj;
    calcBackProject(&image, 1, channels, hist, backproj, ranges, 255, true);

    imshowDebug("BackProj", backproj);
    threshold(backproj, backproj, 24, 255, THRESH_BINARY);
    imshowDebug("backproj_tresh", backproj);

    return backproj;
}

#pragma once
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;

class AutoGrabCut
{
public:
    int skyPercentageInImageForGrabCutInit = 50; // how much sky is expected on top of the image

    int scaling = 8; // image scaling for image pyramid

    Mat bgrSmall;   // image pyramid  1
    Mat bgrSmaller; // image pyramid  2 even smaller

    Mat bgrFiltered;

    Mat bgdModel, fgdModel;
    Mat res;

    Mat detected_edges;
    Mat src_gray;

    const Mat element3 = getStructuringElement(MORPH_RECT, Size(3, 3));

    // some call back for the opencv slider
    static void cb_sky(int value, void *ptr)
    {
        auto T = (AutoGrabCut *)ptr;
        T->skyPercentageInImageForGrabCutInit = value;
    }

    static void cb_scaling(int value, void *ptr)
    {
        auto T = (AutoGrabCut *)ptr;
        T->scaling = max(2, value);
    }

public:
    AutoGrabCut()
    {
        createTrackbar("Sky Percentage initialization", "Result", &skyPercentageInImageForGrabCutInit, 100, cb_sky, this);
        createTrackbar("Image Down Scaling", "Result", &scaling, 16, cb_scaling, this);
    }

    /**
     * @brief Run the full sky segmentation on RGB image
     *
     * @param imageInputBGR
     * @return Mat  Binary mask image, 255 in the sky, 0 otherwise
     */
    Mat runAutoSkySegmentation(const Mat &imageInputBGR);
};
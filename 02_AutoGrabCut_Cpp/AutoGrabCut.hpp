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

    int scaling = 4; // image scaling for image pyramid

    Mat bgrSmall;   // image pyramid  1
    Mat bgrSmaller; // image pyramid  2 even smaller

    Mat bgrFiltered;

    Mat bgdModel, fgdModel;
    Mat res;

    Mat detected_edges;
    Mat src_gray;

    Mat element7 = getStructuringElement(MORPH_RECT, Size(7, 7));
    Mat element5 = getStructuringElement(MORPH_RECT, Size(5, 5));
    Mat element3 = getStructuringElement(MORPH_RECT, Size(3, 3));

    // some call back for the opencv slider
    static void cb_sky(int value, void *ptr)
    {
        auto T = (AutoGrabCut *)ptr;
        T->skyPercentageInImageForGrabCutInit = value;
    }

    // static void cb_generic_forceImpair(int value, void *ptr)
    // {
    //     auto T = (AutoGrabCut *)ptr;
    //     if (value % 2 == 0)
    //         value += 1;
    //     T->genericInt = max(3, value);
    // }

    // static void cb_generic(int value, void *ptr)
    // {
    //     auto T = (AutoGrabCut *)ptr;
    //     T->genericInt = max(1, value);
    // }

    // static void cb_generic2(int value, void *ptr)
    // {
    //     auto T = (AutoGrabCut *)ptr;
    //     T->genericInt2 = max(1, value);
    // }

    static void cb_scaling(int value, void *ptr)
    {
        auto T = (AutoGrabCut *)ptr;
        T->scaling = max(2, value);
    }

public:
    AutoGrabCut()
    {
        createTrackbar("Sky Percentage initialization", "Result", &skyPercentageInImageForGrabCutInit, 100, cb_sky, this);
        // createTrackbar("Generic Int", "Result", &genericInt, 255, cb_generic, this);
        // createTrackbar("Generic Int2", "Result", &genericInt2, 255, cb_generic2, this);
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
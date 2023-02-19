#include "AutoGrabCut.hpp"
#include "utils.hpp"


/**
 * @brief Run the full sky segmentation on RGB image
 * 
 * @param imageInputBGR 
 * @return Mat  Binary mask image, 255 in the sky, 0 otherwise
 */
Mat AutoGrabCut::runAutoSkySegmentation(const Mat &imageInputBGR)
{

    cvtColor(imageInputBGR, src_gray, COLOR_BGR2GRAY);
    Mat edges = detectEdges(src_gray);
    imshowDebug("edges", edges);

    int pyramideScale1 = scaling;
    int pyramideScale2 = scaling / 2;

    resize(imageInputBGR, bgrSmall, {imageInputBGR.cols / pyramideScale1, imageInputBGR.rows / pyramideScale1}, 0, 0, INTER_LANCZOS4);
    // blur(bgrSmall, bgrSmall, {3, 3});
    // medianBlur(bgrSmall, bgrSmall, 3);
    imshowDebug("bgrSmall", bgrSmall);

    resize(bgrSmall, bgrSmaller, {bgrSmall.cols / pyramideScale2, bgrSmall.rows / pyramideScale2}, 0, 0, INTER_LANCZOS4);

    imshowDebug("bgrSmall", bgrSmaller);
    // medianBlur(bgrSmaller, bgrSmaller, 3);
    // blur(bgrSmaller, bgrSmaller, {3, 3});

    // take first X% as sky region init
    cv::Mat mask = cv::Mat::zeros(cv::Size(bgrSmaller.cols, bgrSmaller.rows), CV_8UC1);
    mask = GC_PR_BGD; // probable background == not sky
    Rect rect(0, 0, bgrSmaller.cols, bgrSmaller.rows * skyPercentageInImageForGrabCutInit / 100.);
    rectangle(mask, rect, GC_PR_FGD, -1); // set the top as probable foreground == sky

    // remove edges from probabble sky
    Mat edges_small;
    resize(edges, edges_small, {mask.cols, mask.rows}, 0, 0, INTER_NEAREST);
    // mask should be GC_PR_BGD = 2,  //!< a possible background pixel where there is an edge
    mask = mask - edges_small + edges_small / 128;

    imshowDebug("mask_init", mask * 64);
    grabCut(bgrSmaller, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
    imshowDebug("mask_after_grabCut", mask * 64);

    // histogramTest(imageInputBGR, mask);

    imshowDebug("pass1", getImageWithMaskOverlay(imageInputBGR, mask));

    resize(mask, mask, {bgrSmall.cols, bgrSmall.rows}, 0, 0, INTER_NEAREST);

    erode(mask, mask, element3);

    resize(edges, edges_small, {mask.cols, mask.rows}, 0, 0, INTER_NEAREST);
    // mask should be GC_PR_BGD = 2,  //!< a possible background pixel where there is an edge
    mask = mask - edges_small + edges_small / 128;

    erode(mask, mask, element3);
    imshowDebug("mask eroded", mask * 64);

    imshowDebug("pass1 eroded", getImageWithMaskOverlay(imageInputBGR, mask));

    try
    {
        grabCut(bgrSmall, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
    }
    catch (const std::exception &e)
    {
        grabCut(bgrSmall, mask, rect, bgdModel, fgdModel, 1, GC_EVAL);
    }

    imshowDebug("mask final", mask * 64);

    imshowDebug("pass2", getImageWithMaskOverlay(imageInputBGR, mask));

    mask = mask & 0x01;
    resize(mask, mask, {imageInputBGR.cols, imageInputBGR.rows}, 0, 0, INTER_NEAREST);

    mask = mask - edges;
    mask = mask & 0x01;
    // morphologyEx(mask, mask, cv::MORPH_DILATE, element);
    Mat maskD;
    Mat elementE = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

    blur(mask, mask, {3, 3});
    mask *= 255;

    // dilate(mask, mask, elementE);

    // Mat backProj = histogramBackProject(imageInputBGR, mask);
    // int erosion_size2 = 7;
    // Mat element2 = getStructuringElement(MORPH_RECT, Size(2 * erosion_size2 + 1, 2 * erosion_size2 + 1), Point(erosion_size2, erosion_size2));
    // Mat maskBigger;
    // Mat maskSmaller;
    // dilate(mask, maskBigger, element2);
    // erode(mask, maskSmaller, element2);
    // maskBigger -= maskSmaller;
    // //dilate(edges, maskBigger, element);
    // imshow("maskBorder", (maskBigger &(backProj & 1)) * 255);

    // mask =  (mask - maskBigger) | (maskBigger & (backProj & 1));

    return mask;
};

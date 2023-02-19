#pragma once
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;



/**
 * @brief Compute histogram backprojection map segmentation from image and mask
 * 
 * @param image where the histogram and backprojection is computed
 * @param mask where to compute the histogram
 * @return Mat Binary image where white pixel are likely to belong to the estimated distribution
 */
Mat histogramBackProject(const Mat &image, Mat mask);


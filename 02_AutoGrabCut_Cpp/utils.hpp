#pragma once

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

using namespace std;
using namespace cv;

Mat getImageWithMaskOverlay(const Mat &image, Mat mask, Scalar color = cv::Scalar(0, 0, 0));

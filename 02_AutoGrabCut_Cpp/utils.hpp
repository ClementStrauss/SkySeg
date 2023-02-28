#pragma once

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

#include <cassert>
#include <iostream>
#include <string>


using namespace std;
using namespace cv;


/**
 * @brief Draws a transparent image over a frame Mat.
 *
 * @param frame the frame where the transparent image will be drawn
 * @param transp the Mat image with transparency, read from a PNG image, with the IMREAD_UNCHANGED flag
 * @param xPos x position of the frame image where the image will start.
 * @param yPos y position of the frame image where the image will start.
 */
void drawTransparency(Mat& frame, Mat& transp, int xPos, int yPos);

/**
 * @brief Get the Image With Mask Overlay
 * 
 * @param image rgb image
 * @param mask  binary mask
 * @param color overlay color for mask overlay
 * @return Mat mask overlayed image
 */
Mat getImageWithMaskOverlay(const Mat &image, Mat mask, Scalar color = cv::Scalar(0, 0, 0));

/**
 * @brief Detect edge on image
 * 
 * @param src_gray 
 * @return Mat binary edge mask, 255 on edges
 */
Mat detectEdges(Mat src_gray);

/**
 * @brief imshow overwrite, enable by _DEBUG preprocessor definition
 * 
 * @param name 
 * @param img 
 */
void imshowDebug(String name, const Mat& img);


/**
 * @brief Check if a string ends with suffix
 * 
 * @param s 
 * @param suffix 
 * @return true if s ends with suffix
 */
bool endsWith(const string &s, const string &suffix);
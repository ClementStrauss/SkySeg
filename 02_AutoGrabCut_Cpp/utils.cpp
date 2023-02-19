#include "utils.hpp"

/**
 * @brief Draws a transparent image over a frame Mat.
 *
 * @param frame the frame where the transparent image will be drawn
 * @param transp the Mat image with transparency, read from a PNG image, with the IMREAD_UNCHANGED flag
 * @param xPos x position of the frame image where the image will start.
 * @param yPos y position of the frame image where the image will start.
 */
void drawTransparency(Mat frame, Mat transp, int xPos, int yPos)
{
  Mat mask;
  vector<Mat> layers;

  split(transp, layers); // seperate channels
  Mat rgb[3] = {layers[0], layers[1], layers[2]};
  mask = layers[3];      // png's alpha channel used as mask
  merge(rgb, 3, transp); // put together the RGB channels, now transp insn't transparent
  transp.copyTo(frame.rowRange(yPos, yPos + transp.rows).colRange(xPos, xPos + transp.cols), mask);
}

Mat getImageWithMaskOverlay(const Mat &image, Mat mask, Scalar color)
{
  Mat res = image.clone();

  if (mask.cols != image.cols || mask.rows != image.rows)
  {
    resize(mask, mask, {image.cols, image.rows}, 0, 0, INTER_NEAREST);
  }

  mask = mask & 0x01; // select both GC_FGD    = 1 and  GC_PR_FGD = 3
  Mat negMask = 1 - mask;

  Mat coloredMask(mask.rows, mask.cols, CV_8UC3, color);

  // coloredMask = imread("../gopro_logo_white.png", IMREAD_UNCHANGED);
  // resize(coloredMask,coloredMask,{image.cols, image.rows});

  coloredMask.setTo(cv::Scalar(0, 0, 0), negMask);

  // drawTransparency(res,coloredMask, 0, 0) ;

  // blur(coloredMask, coloredMask, {15, 15});
  //  medianBlur(coloredMask, coloredMask, 5);

  // blur(black, black, {11, 11});

  addWeighted(image, 0.3, res.setTo(cv::Scalar(0, 0, 0), mask), 0.7, 0.0, res); // darken the sky

  addWeighted(res, 1, coloredMask, 0.5, 0.0, res);
  // res = res + coloredMask;

  return res;
}
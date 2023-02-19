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

/**
 * @brief Get the Image With Mask Overlay
 * 
 * @param image rgb image
 * @param mask  binary mask
 * @param color overlay color for mask overlay
 * @return Mat mask overlayed image
 */
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


/**
 * @brief Detect edge on image
 * 
 * @param src_gray 
 * @return Mat binary edge mask, 255 on edges
 */
Mat detectEdges(const Mat &src_gray)
{
  Mat grad_x, grad_y, edges;
  blur(src_gray, src_gray, Size(5, 5));

  // Canny( src_gray, detected_edges, 50, 60);
  // dilate(detected_edges, detected_edges, element);
  // morphologyEx(detected_edges, detected_edges, MORPH_CLOSE, element);
  // imshow("detected_edges", detected_edges);
  // mask = mask - detected_edges;

  Scharr(src_gray, grad_x, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
  Scharr(src_gray, grad_y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
  // converting back to CV_8U
  convertScaleAbs(grad_x, grad_x);
  convertScaleAbs(grad_y, grad_y);
  addWeighted(grad_x, 0.5, grad_y, 0.5, 0, edges);
  edges.convertTo(edges, CV_8U);
  // morphologyEx(sobel, sobel, MORPH_OPEN, element);

  threshold(edges, edges, 64, 255, THRESH_BINARY);

  return edges;
}


/**
 * @brief imshow overwrite, enable by _DEBUG preprocessor definition
 * 
 * @param name 
 * @param img 
 */
void imshowDebug(String name, Mat img)
{
#ifdef _DEBUG
  imshow(name, img);
#endif
}
#include <cassert>
#include <iostream>
#include <string>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>

using namespace std;
using namespace cv;

struct AutoGrabCut
{

  Mat bgrSmall;
  Mat bgrSmaller;
  int pyramideScale1 = 6;
  int pyramideScale2 = 2;
  Mat bgrFiltered;

  Mat bgdModel, fgdModel;
  Mat res;

  Mat detected_edges;
  Mat src_gray;

  Mat grad_x, grad_y, sobel;

  float skyRatioInImageForGrabCutInit = 0.5;

public:
  Mat runGrabCut(Mat imageInputBGR)
  {

    cvtColor(imageInputBGR, src_gray, COLOR_BGR2GRAY);

    resize(imageInputBGR, bgrSmall, {imageInputBGR.cols / pyramideScale1, imageInputBGR.rows / pyramideScale1}, 0, 0, INTER_LINEAR);
    // blur(bgrSmall, bgrSmall, {3, 3});
    medianBlur(bgrSmall, bgrSmall, 3);
    resize(bgrSmall, bgrSmaller, {bgrSmall.cols / pyramideScale2, bgrSmall.rows / pyramideScale2}, 0, 0, INTER_LINEAR);
    // medianBlur(bgrSmaller, bgrSmaller, 3);
    // blur(bgrSmaller, bgrSmaller, {3, 3});

    // take first 33% as sky region init
    // Drawing the Rectangle
    cv::Mat mask = cv::Mat::zeros(cv::Size(bgrSmaller.cols, bgrSmaller.rows), CV_8UC1);
    mask = GC_PR_BGD;

    Rect rect(0, 0, bgrSmaller.cols, bgrSmaller.rows * skyRatioInImageForGrabCutInit);
    rectangle(mask, rect, GC_PR_FGD, -1);

    grabCut(bgrSmaller, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);

    imshow("bgrSmall", bgrSmall);
    imshow("mask", mask * 64);
    int erosion_size = 3;
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
    // erode(mask, mask, element);
    imshow("mask erode", mask * 64);

    resize(mask, mask, {bgrSmall.cols, bgrSmall.rows}, 0, 0, INTER_NEAREST);
    erode(mask, mask, element);

    try
    {
      grabCut(bgrSmall, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
    }
    catch (const std::exception &e)
    {
      grabCut(bgrSmall, mask, rect, bgdModel, fgdModel, 1, GC_EVAL);
    }

    mask = mask & 0x01;
    resize(mask, mask, {imageInputBGR.cols, imageInputBGR.rows}, 0, 0, INTER_LANCZOS4);

    morphologyEx(mask, mask, cv::MORPH_OPEN, element);
    // erode(mask, mask, element);

    blur(src_gray, src_gray, Size(3, 3));

    // Canny( src_gray, detected_edges, 50, 60);
    // dilate(detected_edges, detected_edges, element);
    // morphologyEx(detected_edges, detected_edges, MORPH_CLOSE, element);
    // imshow("detected_edges", detected_edges);
    // mask = mask - detected_edges;

    Sobel(src_gray, grad_x, CV_16S, 1, 0, 5, 1, 0, BORDER_DEFAULT);
    Sobel(src_gray, grad_y, CV_16S, 0, 1, 5, 1, 0, BORDER_DEFAULT);
    // converting back to CV_8U
    convertScaleAbs(grad_x, grad_x);
    convertScaleAbs(grad_y, grad_y);
    addWeighted(grad_x, 0.5, grad_y, 0.5, 0, sobel);
    sobel.convertTo(sobel, CV_8U);
    // morphologyEx(sobel, sobel, MORPH_OPEN, element);

    threshold(sobel, sobel, 128, 255, THRESH_BINARY);
    imshow("sobel2", sobel);

    mask = mask - sobel;

    blur(mask, mask, {5, 5});

    Mat darkenImage(mask.rows, mask.cols, CV_8UC3, cv::Scalar(255, 255, 255));
    darkenImage.setTo(cv::Scalar(0, 0, 0), mask);

    Mat coloredMask(mask.rows, mask.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    coloredMask.setTo(cv::Scalar(0, 255, 0), mask);

    // blur(coloredMask, coloredMask, {15, 15});
    //  medianBlur(coloredMask, coloredMask, 5);

    // blur(black, black, {11, 11});

    // multiply(imageInputBGR, coloredMask, imageInputBGR, 1. / 255);
    //   imshow("bgr", bgr);

    addWeighted(coloredMask, 0.8, imageInputBGR, 0.7, 0.0, res);

    // imshow("mask", mask * 64);
    return res;
  }
};

int main(int argc, char *argv[])
{
  string filename = "/home/cstrauss/PERSO/StreamFlow/lofoten.mp4";
  if (argc > 1)
  {
    // The first argument (argv[0]) is the name of the program
    // The second argument (argv[1]) is the first command-line argument
    std::cout << "The argument is: " << argv[1] << std::endl;
    filename = argv[1];
  }
  else
  {
    std::cout << "No argument provided." << std::endl;
  }

  VideoCapture cap;

  cap.open(filename);
  cout << "open video" << endl;

  if (!cap.isOpened())
  {
    cerr << "cannot open video " << filename << endl;
    throw;
  }

  char key = 0;
  AutoGrabCut skyGrabCut;
  Mat result;
  while (key != 'q')
  {
    Mat image;
    cap.read(image);
    cap.read(image);

    if (image.empty())
      break;

    imshow("source", image);
    result = skyGrabCut.runGrabCut(image);
    imshow("result", result);

    key = waitKey(1);
  }

  return 0;
}

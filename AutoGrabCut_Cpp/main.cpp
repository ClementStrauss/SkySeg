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

  float skyRatioInImageForGrabCutInit = 0.3;

public:
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

  Mat getImageWithMask(const Mat &image, Mat mask)
  {
    Mat res = image.clone();

    if (mask.cols != image.cols || mask.rows != image.rows)
    {
      resize(mask, mask, {image.cols, image.rows}, 0, 0, INTER_LINEAR);
    }

    mask = mask & 0x01; // select both GC_FGD    = 1 and  GC_PR_FGD = 3

    Mat coloredMask(mask.rows, mask.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    coloredMask.setTo(cv::Scalar(0, 255, 0), mask);

    // blur(coloredMask, coloredMask, {15, 15});
    //  medianBlur(coloredMask, coloredMask, 5);

    // blur(black, black, {11, 11});

    // multiply(imageInputBGR, coloredMask, imageInputBGR, 1. / 255);
    //   imshow("bgr", bgr);

    addWeighted(image, 0.8, coloredMask, 0.7, 0.0, res);

    return res;
  }

  void histogramTest(const Mat &image, Mat mask)
  {
    Mat res = image.clone();
    Mat colorSpaced;
    // cvtColor(image, colorSpaced, COLOR_BGR2YCrCb);

    if (mask.cols != image.cols || mask.rows != image.rows)
    {
      resize(mask, mask, {image.cols, image.rows}, 0, 0, INTER_LINEAR);
    }

    mask = mask & 0x01; // select both GC_FGD    = 1 and  GC_PR_FGD = 3

    int binSize = 64;

    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 1};

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int c0Bins = binSize, c1Bins = binSize, c2Bins = binSize;
    int histSize[] = {c0Bins, c1Bins};

    float c0Ranges[] = {0, 256}; // hue varies from 0 to 179, see cvtColor
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float c1Ranges[] = {0, 256};
    float c2Ranges[] = {0, 256};
    const float *ranges[] = {c0Ranges, c1Ranges};
    Mat hist;

    calcHist(&image, 1, channels, mask,
             hist, 2, histSize, ranges,
             true, // the histogram is uniform
             false);

    normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat());

    Mat backproj;
    calcBackProject(&image, 1, channels, hist, backproj, ranges, 1, true);

    imshow("BackProj", backproj);
    threshold(backproj, backproj, 8, 255, THRESH_BINARY);
    imshow("backproj_tresh", backproj);
  }

  Mat runGrabCut(const Mat &imageInputBGR)
  {

    cvtColor(imageInputBGR, src_gray, COLOR_BGR2GRAY);
    Mat edges = detectEdges(src_gray);
    imshow("edges", edges);

    resize(imageInputBGR, bgrSmall, {imageInputBGR.cols / pyramideScale1, imageInputBGR.rows / pyramideScale1}, 0, 0, INTER_LINEAR);
    // blur(bgrSmall, bgrSmall, {3, 3});
    medianBlur(bgrSmall, bgrSmall, 3);
    imshow("bgrSmall", bgrSmall);

    resize(bgrSmall, bgrSmaller, {bgrSmall.cols / pyramideScale2, bgrSmall.rows / pyramideScale2}, 0, 0, INTER_LINEAR);
    // medianBlur(bgrSmaller, bgrSmaller, 3);
    // blur(bgrSmaller, bgrSmaller, {3, 3});

    // take first X% as sky region init
    cv::Mat mask = cv::Mat::zeros(cv::Size(bgrSmaller.cols, bgrSmaller.rows), CV_8UC1);
    mask = GC_PR_BGD; // probable background == not sky
    Rect rect(0, 0, bgrSmaller.cols, bgrSmaller.rows * skyRatioInImageForGrabCutInit);
    rectangle(mask, rect, GC_PR_FGD, -1); // set the top as probable foreground == sky

    // remove edges from probabble sky
    Mat edges_small;
    resize(edges, edges_small, {mask.cols, mask.rows}, 0, 0, INTER_NEAREST);
    // mask should be GC_PR_BGD = 2,  //!< a possible background pixel where there is an edge
    mask = mask - edges_small + edges_small / 128;

    imshow("mask_init", mask * 64);
    grabCut(bgrSmaller, mask, rect, bgdModel, fgdModel, 2, GC_INIT_WITH_MASK);
    imshow("mask_after_grabCut", mask * 64);

    // histogramTest(imageInputBGR, mask);

    imshow("pass1", getImageWithMask(imageInputBGR, mask));

    resize(mask, mask, {bgrSmall.cols, bgrSmall.rows}, 0, 0, INTER_NEAREST);

    int erosion_size = 3;
    Mat element = getStructuringElement(MORPH_RECT, Size(2 * erosion_size + 1, 2 * erosion_size + 1), Point(erosion_size, erosion_size));
    erode(mask, mask, element);
    imshow("mask eroded", mask * 64);

    histogramTest(imageInputBGR, mask);

    imshow("pass1.1", getImageWithMask(imageInputBGR, mask));

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

    mask = mask - edges;

    blur(mask, mask, {5, 5});

    res = getImageWithMask(imageInputBGR, mask);

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
